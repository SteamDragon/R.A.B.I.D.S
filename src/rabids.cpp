#include "rabids.h"

#include <filesystem>
#include <random>
#include <vector>

#include "stdiohandler.h"

char_array charset()
{
	return char_array(
		{'0', '1', '2', '3', '4',
		 '5', '6', '7', '8', '9',
		 'A', 'B', 'C', 'D', 'E', 'F',
		 'G', 'H', 'I', 'J', 'K',
		 'L', 'M', 'N', 'O', 'P',
		 'Q', 'R', 'S', 'T', 'U',
		 'V', 'W', 'X', 'Y', 'Z',
		 'a', 'b', 'c', 'd', 'e', 'f',
		 'g', 'h', 'i', 'j', 'k',
		 'l', 'm', 'n', 'o', 'p',
		 'q', 'r', 's', 't', 'u',
		 'v', 'w', 'x', 'y', 'z'});
};

namespace Command
{
	using Verb = std::function<
		void(
			RABIDS &,
			SleepyDiscord::Interaction &)>;
	struct Command
	{
		std::string name;
		Verb verb;
	};
	using MappedCommands = std::unordered_map<std::string, Command>;
	using MappedCommand = MappedCommands::value_type;
	static MappedCommands all;
	static void addCommand(Command command)
	{
		all.try_emplace(command.name, command);
	}
}

std::string CurrentTime()
{
	char buf[80];
	time_t current_time;
	struct tm local_time;
	time(&current_time);
	localtime_r(&current_time, &local_time);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &local_time);

	return buf;
}

void PBKDF2_HMAC_SHA_512(const char *pass, const unsigned char *salt, int32_t iterations, uint32_t outputBytes, char *hexResult)
{
	std::vector<unsigned char> digest(outputBytes);
	PKCS5_PBKDF2_HMAC(pass, strlen(pass), salt, strlen((char *)salt), iterations, EVP_sha512(), outputBytes, digest.data());
	for (unsigned int i = 0; i < digest.size(); i++)
	{
		sprintf(hexResult + (i * 2), "%02x", 255 & digest[i]);
	}
}

std::string random_string(size_t length, std::function<char(void)> const & rand_char)
{
	std::string str(length, 0);
	std::generate_n(str.begin(), length, rand_char);
	return str;
}

void RABIDS::onInteraction(SleepyDiscord::Interaction interaction)
{
	auto foundCommand = Command::all.find(interaction.data.name);
	if (foundCommand == Command::all.end())
	{
		SleepyDiscord::Interaction::Response<> response;
		response.type = SleepyDiscord::Interaction::CallbackType::ChannelMessageWithSource;
		response.data.content = "Couldn't find command";
		response.data.flags = SleepyDiscord::InteractionAppCommandCallbackData::Flags::Ephemeral;
		createInteractionResponse(interaction, interaction.token, response);
		return;
	}
	foundCommand->second.verb(*this, interaction);
}

void RABIDS::onMessage(SleepyDiscord::Message message)
{
	std::string channel = message.channelID;
	std::string server = message.serverID;

	if (channel != configuration->ChannelId())
	{
		return;
	}

	if (message.startsWith("help"))
	{
		LOG(info) << "Help Called for " << message.author.username;
		sendMessage(message.channelID, configuration->GetTextMessages().HelpMessage());
		return;
	}

	if (message.startsWith("version"))
	{
		LOG(info) << "Version Called for " << message.author.username;
		std::string fullMessage = configuration->GetTextMessages().BotVersionString() + configuration->getVersion();
		sendMessage(message.channelID, fullMessage);
		return;
	}

	if (message.startsWith("start"))
	{
		if (startServer())
		{
			LOG(info) << configuration->GetTextMessages().ServerStartedMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerStartedMessage());
		}
		else
		{
			LOG(error) << configuration->GetTextMessages().ServerFailedToStartMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerFailedToStartMessage());
		}

		return;
	}

	if (message.startsWith("stop"))
	{
		if (stopServer())
		{
			LOG(info) << configuration->GetTextMessages().ServerStoppedMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerStoppedMessage());
		}
		else
		{
			LOG(error) << configuration->GetTextMessages().ServerFailedToStopMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerFailedToStopMessage());
		}

		return;
	}

	if (message.startsWith("restartTimeout"))
	{
		try
		{
			std::string s = message.content.substr(15);
			long long restartTimeoutInternal = std::stoll(s) * 3600000;
			configuration->SetRestartInterval(restartTimeoutInternal);

			LOG(info) << fmt::format(configuration->RestartTimeoutTemplate(), s);
			sendMessage(message.channelID, fmt::format(configuration->RestartTimeoutTemplate(), s));
		}
		catch (...)
		{
			LOG(error) << configuration->GetTextMessages().SetRestartTimeoutFailedMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().SetRestartTimeoutFailedMessage());
		}

		return;
	}

	if (message.startsWith("restart"))
	{
		if (stopServer())
		{
			LOG(info) << configuration->GetTextMessages().ServerStoppedMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerStoppedMessage());

			if (startServer())
			{
				LOG(info) << configuration->GetTextMessages().ServerStartedMessage();
				sendMessage(message.channelID, configuration->GetTextMessages().ServerStartedMessage());
			}
			else
			{
				LOG(error) << configuration->GetTextMessages().ServerFailedToStartMessage();
				sendMessage(message.channelID, configuration->GetTextMessages().ServerFailedToStartMessage());
			}
		}
		else
		{
			LOG(error) << configuration->GetTextMessages().ServerFailedToStopMessage();
			sendMessage(message.channelID, configuration->GetTextMessages().ServerFailedToStopMessage());
		}

		return;
	}

	if (message.startsWith("insertDowntime"))
	{
		try
		{
			std::string s = message.content.substr(14);
			std::string username = message.mentions[0].username;
			std::string actorname = s;
			unsigned long pos = actorname.find("> ");
			actorname.erase(0, pos + 1);
			trim(username);
			trim(actorname);
			std::string mentionId = message.mentions[0].ID;
			tracker.InsertEntry(mentionId, username, actorname);
			sendMessage(message.channelID, "Done");
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = configuration->DowntimeChannelId();
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = configuration->DowntimeMessageId();
			editMessage(channelId, dtMessageId, tracker.FormTable());
			LOG(info) << "Inserting Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Inserting Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->GetTextMessages().GeneralError());
		}
		return;
	}

	if (message.startsWith("update"))
	{
		try
		{
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = configuration->DowntimeChannelId();
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = configuration->DowntimeMessageId();
			editMessage(channelId, dtMessageId, tracker.FormTable());
			LOG(info) << "Downtime Table Updated";
			sendMessage(message.channelID, "Done");
		}
		catch (...)
		{
			LOG(error) << "Downtime Table Update Failed";
			sendMessage(message.channelID, configuration->GetTextMessages().GeneralError());
		}
		return;
	}

	if (message.startsWith("use"))
	{
		try
		{
			std::string msg = message.content.substr(4);
			Record records;

			std::istringstream f(msg);
			std::string s;
			while (std::getline(f, s, ' '))
			{
				records.push_back(s);
			}

			LOG(info) << msg;
			LOG(info) << records[0];
			LOG(info) << records[1];
			int number = std::stoi(records[1]);
			auto mention = message.mentions[0];
			std::string mentionId = mention.ID;
			tracker.ReduceDowntimes(mentionId, number);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = configuration->DowntimeChannelId();
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = configuration->DowntimeMessageId();
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Done");
			LOG(info) << "Usage of Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Usage of Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->GetTextMessages().GeneralError());
		}
		return;
	}

	if (message.startsWith("addDowntime"))
	{
		try
		{
			std::string msg = message.content.substr(12);
			Record records;
			std::string delimiter("\n");
			int number = std::stoi(msg.substr(0, msg.find(delimiter)));
			for (const auto& mention : message.mentions)
			{
				records.push_back(mention.ID);
			}

			if(records.size() == 0)
			{
				LOG(info) << "Adding of Downtime failed: no Data Entered";
				sendMessage(message.channelID, "Invalid Request");
				return;
			}

			tracker.AddDowntimes(records, number);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = configuration->DowntimeChannelId();
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = configuration->DowntimeMessageId();
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Healing: " + tracker.GetLastHealList());
			sendMessage(message.channelID, "Cryo: " + tracker.GetLastCryoList());
			sendMessage(message.channelID, "Done");
			LOG(info) << "Adding of Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Adding of Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->GetTextMessages().DowntimeError());
		}
		
		return;
	}

	if (message.startsWith("heal"))
	{
		try
		{
			std::string msg = message.content.substr(5);
			Record records;
			std::istringstream f(msg);
			std::string s;
			int cryo;
			while (std::getline(f, s, ' '))
			{
				records.push_back(s);
			}

			if (records.size() < 2)
			{
				sendMessage(message.channelID, configuration->GetTextMessages().ReadManualError());
			}

			int index = 1;
			while (records[index].empty())
			{
				index++;
			}

			int number = std::stoi(records[index]);
			if (records.size() < 3)
			{
				cryo = 0;
			}
			else
			{

				while (records[index + 1].empty())
				{
					index++;
				}

				cryo = std::stoi(records[index + 1]);
			}
			
			std::string mentionId = message.mentions[0].ID;
			tracker.AddHealing(mentionId, number, cryo);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = configuration->DowntimeChannelId();
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = configuration->DowntimeMessageId();
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Done");
			LOG(info) << "Heal adding succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Heal adding Failed: " << message.content;
			sendMessage(message.channelID, configuration->GetTextMessages().DowntimeError());
		}
		return;
	}
}

std::string RABIDS::Register(SleepyDiscord::User user, const std::string& password)
{
	std::string result;
	try
	{
		LOG(info) << "Registration Called for " << user.username;
		std::string salt = user.ID;
		std::string pwd = password;
		char hexResult[2 * SHA512_DIGEST_LENGTH + 1];
		memset(hexResult, 0, sizeof(hexResult));
		PBKDF2_HMAC_SHA_512(pwd.c_str(), reinterpret_cast<const unsigned char *>(salt.c_str()), 1000, SHA512_DIGEST_LENGTH, hexResult);

		sync->enqueueRegistration(user.ID, user.username, hexResult);

		if (checkServerStatus() == ServerStatus::READY)
		{
			result = configuration->TimedRegistrationString() + std::to_string(numberOfPlayers);
		}
		else
		{
			result = configuration->GetTextMessages().SucceedRegistrationString();
		}
	}
	catch (...)
	{
		result = "Failed to create user";
	}

	return result;
}

bool RABIDS::startServer()
{
	LOG(info) << "Starting Server";
	pid = createChild(configuration->ApplicationAndArguments());
	LOG(info) << "Process started with PID: " << pid;
	return pid != -1;
}

bool RABIDS::stopServer()
{
	if (checkServerStatus() == ServerStatus::READY)
	{
		kill(pid, SIGINT);
		pid = -1;
		return true;
	}

	return false;
}

ServerStatus RABIDS::checkServerStatus()
{
	if (updatingDB)
	{
		return ServerStatus::DBUPDATE;
	}

	auto v = configuration->ApplicationAndArguments();
	auto space = [](std::string a, const std::string& b)
	{
		return std::move(a) + ' ' + b;
	};
	std::string fullCommand = std::accumulate(std::next(v.begin()), v.end(),
											  v[0], space);
	std::string results = exec("ps -ax | grep \"" + fullCommand + "\" | wc -l");

	if (std::stoi(results) - 1 > 1)
	{
		return ServerStatus::READY;
	}
	else
	{
		return ServerStatus::OFF;
	}
}

void RABIDS::scheduleStatusUpdate()
{
	this->schedule([this]()
				   {
		ServerStatus status = checkServerStatus();
		switch (status)
		{
		case ServerStatus::OFF:
			this->updateStatus(configuration->GetTextMessages().ServerOffMessage(), 0, SleepyDiscord::Status::online, false);
			break;
		case ServerStatus::READY:
			this->updateStatus(configuration->GetTextMessages().ReadyMessage() + std::to_string(numberOfPlayers), 0, SleepyDiscord::Status::online, false);
			break;
		case ServerStatus::DBUPDATE:
		LOG(info)<<"Stop Server";
			stopServer();
			updatingDB = true;
		LOG(info)<<"Update Status";
			this->updateStatus(configuration->GetTextMessages().DbUpdateMessage(), 0, SleepyDiscord::Status::online, false);
		LOG(info)<<"restartServer";
			updatingDB = false;
		LOG(info)<<"startServer";
			startServer();
			break;
		default:
			break;
		}


		this->scheduleStatusUpdate();
		if (checkServerStatus() == ServerStatus::READY)
		{
			int count = 0;
			if (sync && sync->loadPlayerCount(count))
			{
				numberOfPlayers = count;
			}
			else
			{
				numberOfPlayers = 0;
			}
		}
		else
		{
			numberOfPlayers = 0;
		} },
				   3000);
}

void RABIDS::startClient(config& externalConfig)
{
	sync = new SyncEngine(
		externalConfig.BridgeDir(),
		externalConfig.FoundryUrl(),
		externalConfig.FoundryApiKey()
	);

	configuration = &externalConfig;

	tracker.init();

	this->schedule([this]()
				   {
						   LOG(info) << "Schedule Status Update";
			this->updateStatus(configuration->GetTextMessages().ServerOffMessage(), 0, SleepyDiscord::Status::online, false);
			this->scheduleStatusUpdate(); },
				   500);
	this->schedule([this]()
				   {
						   LOG(info) << "Restart Scheduled";
						   scheduleRestart(); },
				   configuration->RestartInterval() - configuration->AlertInterval());

	this->run();
	exit(0);
}

void RABIDS::onReady(SleepyDiscord::Ready ready)
{
	static int connectCount = 0;
	connectCount += 1;
	if (connectCount == 1)
	{
		onFirstConnect();
	}
}

void RABIDS::onFirstConnect()
{
	{
		Command::Command reg{
			"reg", [](
					   RABIDS &client,
					   SleepyDiscord::Interaction &interaction)
			{
				std::string password;
				for (auto &option : interaction.data.options)
				{
					std::string pwd;
					if (option.get(pwd))
					{
						password = pwd;
						break;
					}
				}
				std::string responseMessage = client.Register(interaction.user, password);

				SleepyDiscord::Interaction::Response<> response;
				response.type = SleepyDiscord::Interaction::CallbackType::ChannelMessageWithSource;
				response.data.content = responseMessage;
				client.createInteractionResponse(interaction.ID, interaction.token, response);
				return;
			}};

		std::vector<SleepyDiscord::AppCommand::Option> options;

		SleepyDiscord::AppCommand::Option password;
		password.name = "password";
		password.isRequired = true;
		password.description = "Password to register or change password";
		password.type = SleepyDiscord::AppCommand::Option::TypeHelper<std::string>::getType();
		options.push_back(std::move(password));

		createGlobalAppCommand(getID(), reg.name, "Perform registration", std::move(options));
		Command::addCommand(reg);
	}
}

void RABIDS::SendErrorMessage()
{
}

void RABIDS::scheduleRestart()
{
	if (checkServerStatus() != ServerStatus::READY)
	{
		this->schedule([this]()
					   { scheduleRestart(); },
					   configuration->AlertInterval());
		return;
	}

	if (numberOfPlayers == 0)
	{
		sendMessage(configuration->AlertChannelId(), configuration->GetTextMessages().AlertMessage());
		this->schedule([this]()
					   {
		try
		{
			stopServer();
			startServer();
			this->schedule([this]() { scheduleRestart(); }, configuration->RestartInterval() - configuration->AlertInterval());
		}
		catch (const std::exception& e)
		{
			LOG(warning) << "FAILED TO RESTART: " << e.what();
		}
		catch(...)
		{
			LOG(warning) << "FAILED TO RESTART: " << "Something strange happened";
		} },
					   configuration->AlertInterval());
		return;
	}
	else
	{
		this->schedule([this]()
					   { scheduleRestart(); },
					   configuration->AlertInterval());
		return;
	}
}

int RABIDS::createChild(std::vector<std::string> arguments)
{
	int pid = fork();
	if (pid == 0)
	{
		int length = (int)arguments.size();
		const char **argv = new const char *[length + 1];
		for (int j = 0; j < length; ++j)
			argv[j] = arguments[j].c_str();
		argv[length] = NULL;
		StdErrHandler err([](const char *line)
						  { BOOST_LOG_TRIVIAL(error) << line; });
		StdOutHandler out([](const char *line)
						  {
						  if (std::string(line).find("warn") != std::string::npos)
						  {
							  BOOST_LOG_TRIVIAL(warning) << line;
						  }
						  else
						  {
							  if (std::string(line).find("Players:**") == std::string::npos)
							  {
								  BOOST_LOG_TRIVIAL(info) << line;
							  }
						   } });
		execv(arguments[0].c_str(), (char **)argv);

		LOG(error) << "Exec error: " << errno << ", " << strerror(errno);

		exit(1);
	}
	else if (pid > 0)
	{
		signal(SIGCHLD, SIG_IGN);
		return pid;
	}
	else
	{
		LOG(error) << "Fork error: " << errno << ", " << strerror(errno);
		return -1;
	}
}

std::string RABIDS::exec(std::string command)
{
	char buffer[128];
	std::string result = "";

	FILE *pipe = popen(command.c_str(), "r");
	if (!pipe)
	{
		return "popen failed!";
	}

	while (!feof(pipe))
	{

		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}

	pclose(pipe);
	return result;
}
