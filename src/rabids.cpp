#include "rabids.h"
#include "stdiohandler.h"

char_array charset()
{
	// Change this to suit
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

template <typename T>
void pop_front(std::vector<T> &vec)
{
	assert(!vec.empty());
	vec.front() = std::move(vec.back());
	vec.pop_back();
}

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
		all.emplace(command.name, command);
	}
}

/// <summary>
/// Get current date/time, format is YYYY-MM-DD HH:mm:ss
/// </summary>
/// <returns>Time string</returns>
std::string CurrentTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

	return buf;
}

void PBKDF2_HMAC_SHA_512(const char *pass, const unsigned char *salt, int32_t iterations, uint32_t outputBytes, char *hexResult)
{
	unsigned int i;
	unsigned char digest[outputBytes];
	PKCS5_PBKDF2_HMAC(pass, strlen(pass), salt, strlen((char *)salt), iterations, EVP_sha512(), outputBytes, digest);
	for (i = 0; i < sizeof(digest); i++)
	{
		sprintf(hexResult + (i * 2), "%02x", 255 & digest[i]);
	};
}

std::string random_string(size_t length, std::function<char(void)> rand_char)
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
		// not found
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
		sendMessage(message.channelID, configuration->HelpMessage());
		return;
	}

	if (message.startsWith("version"))
	{
		LOG(info) << "Version Called for " << message.author.username;
		std::string fullMessage = configuration->BotVersionString().append(configuration->Version);
		sendMessage(message.channelID, fullMessage);
		return;
	}

	if (message.startsWith("start"))
	{
		if (startServer())
		{
			LOG(info) << configuration->ServerStartedMessage();
			sendMessage(message.channelID, configuration->ServerStartedMessage());
		}
		else
		{
			LOG(error) << configuration->ServerFailedToStartMessage();
			sendMessage(message.channelID, configuration->ServerFailedToStartMessage());
		}

		return;
	}

	if (message.startsWith("stop"))
	{
		if (stopServer())
		{
			LOG(info) << configuration->ServerStoppedMessage();
			sendMessage(message.channelID, configuration->ServerStoppedMessage());
		}
		else
		{
			LOG(error) << configuration->ServerFailedToStopMessage();
			sendMessage(message.channelID, configuration->ServerFailedToStopMessage());
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
			LOG(error) << configuration->SetRestartTimeoutFailedMessage();
			sendMessage(message.channelID, configuration->SetRestartTimeoutFailedMessage());
		}

		return;
	}

	if (message.startsWith("restart"))
	{
		if (stopServer())
		{
			LOG(info) << configuration->ServerStoppedMessage();
			sendMessage(message.channelID, configuration->ServerStoppedMessage());

			if (startServer())
			{
				LOG(info) << configuration->ServerStartedMessage();
				sendMessage(message.channelID, configuration->ServerStartedMessage());
			}
			else
			{
				LOG(error) << configuration->ServerFailedToStartMessage();
				sendMessage(message.channelID, configuration->ServerFailedToStartMessage());
			}
		}
		else
		{
			LOG(error) << configuration->ServerFailedToStopMessage();
			sendMessage(message.channelID, configuration->ServerFailedToStopMessage());
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
			int pos = actorname.find("> ");
			actorname.erase(0, pos + 1);
			trim(username);
			trim(actorname);
			tracker.InsertEntry(message.mentions[0].ID, username, actorname);
			sendMessage(message.channelID, "Done");
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = downtimeChannel;
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = downtimeMessage;
			editMessage(channelId, dtMessageId, tracker.FormTable());
			LOG(info) << "Inserting Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Inserting Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->GeneralError());
		}
		return;
	}

	if (message.startsWith("update"))
	{
		try
		{
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = downtimeChannel;
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = downtimeMessage;
			editMessage(channelId, dtMessageId, tracker.FormTable());
			LOG(info) << "Downtime Table Updated";
			sendMessage(message.channelID, "Done");
		}
		catch (...)
		{
			LOG(error) << "Downtime Table Update Failed";
			sendMessage(message.channelID, configuration->GeneralError());
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
			tracker.ReduceDowntimes(mention.ID, number);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = downtimeChannel;
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = downtimeMessage;
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Done");
			LOG(info) << "Usage of Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Usage of Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->GeneralError());
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
			for (auto mention : message.mentions)
			{
				records.push_back(mention.ID);
			}

			tracker.AddDowntimes(records, number);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = downtimeChannel;
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = downtimeMessage;
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Healing: " + tracker.LastHealList);
			sendMessage(message.channelID, "Cryo: " + tracker.LastCryoList);
			sendMessage(message.channelID, "Done");
			LOG(info) << "Adding of Downtime succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Adding of Downtime Failed: " << message.content;
			sendMessage(message.channelID, configuration->DowntimeError());
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
				sendMessage(message.channelID, configuration->ReadManualError());
			}

			int number = std::stoi(records[1]);
			if (records.size() < 3)
			{
				cryo = 0;
			}
			else
			{
				cryo = std::stoi(records[2]);
			}

			tracker.AddHealing(message.mentions[0].ID, number, cryo);
			SleepyDiscord::Snowflake<SleepyDiscord::Channel>
				channelId = downtimeChannel;
			SleepyDiscord::Snowflake<SleepyDiscord::Message> dtMessageId = downtimeMessage;
			editMessage(channelId, dtMessageId, tracker.FormTable());
			sendMessage(message.channelID, "Done");

			LOG(info) << "Heal adding succeed: " << message.content;
		}
		catch (...)
		{
			LOG(error) << "Heal adding Failed: " << message.content;
			sendMessage(message.channelID, configuration->DowntimeError());
		}
		return;
	}
}

std::string RABIDS::Register(SleepyDiscord::User user, std::string password)
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
		json userJson = RABIDS::GetUserToUpdate(salt);
		if (userJson["name"] == "name")
		{
			userJson["name"] = "-- " + user.username + " --";
		}

		userJson["password"] = hexResult;
		users.push_back(userJson);

		json actor = RABIDS::GetActorToUpdate(userJson);
		if (actor.dump() != "{\"status\":\"null\"}")
		{
			actors.push_back(RABIDS::GetActorToUpdate(userJson));
		}

		if (checkServerStatus() == ServerStatus::READY)
		{
			result = configuration->TimedRegistrationString() + std::to_string(numberOfPlayers);
		}
		else
		{
			result = configuration->SucceedRegistrationString();
		}

		dbChanged = true;
		if (!users.empty())
		{
			std::ofstream dataUsers(tempUsersInternal, std::ios::out | std::ios::app);
			for (json user : users)
			{
				dataUsers << user.dump();
			}

			dataUsers.close();
		}

		if (!actors.empty())
		{
			std::ofstream dataActors(tempActorsInternal, std::ios::out | std::ios::app);
			for (json actor : actors)
			{
				dataActors << actor.dump();
			}

			dataActors.close();
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
	auto space = [](std::string a, std::string b)
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
		case OFF:
			this->updateStatus(configuration->ServerOffMessage(), 0, SleepyDiscord::Status::online, false);
			break;
		case READY:
			this->updateStatus(configuration->ReadyMessage() + std::to_string(numberOfPlayers), 0, SleepyDiscord::Status::online, false);
			break;
		case DBUPDATE:
		LOG(info)<<"Stop Server";
			stopServer();
			updatingDB = true;
		LOG(info)<<"Update Status";
			this->updateStatus(configuration->DbUpdateMessage(), 0, SleepyDiscord::Status::online, false);
		LOG(info)<<"updateLocalDBInstance";
			updateLocalDBInstance();
		LOG(info)<<"updateDB";
			updateDB();
		LOG(info)<<"uploadDB";
			uploadDBUsersDb();
			uploadDBActorsDb();
			updatingDB = false;
		LOG(info)<<"startServer";
			startServer();
			break;
		default:
			break;
		}

		if(checkServerStatus() == ServerStatus::OFF && dbChanged)
		{
			updatingDB = true;
		}

		if(numberOfPlayers == 0 && dbChanged)
		{
			if(counter == 40)
			{
				updatingDB = true;
				counter = 0;
			}

			if(!updatingDB)
			{
				counter++;
			}
		}


		this->scheduleStatusUpdate();
		if (checkServerStatus() == ServerStatus::READY)
		{
			std::ifstream input( configuration->PlayerCountFile(), std::ios_base::in);
			std::string line;
			std::getline(input, line);
			input.close();
			if(line != "")
			{
				numberOfPlayers = std::stoi(line);
			}
			else
			{
				numberOfPlayers = 0;
			}

			input.close();
		}
		else
		{
			numberOfPlayers = 0;
		} },
				   3000);
}

void RABIDS::startClient(config externalConfig)
{
	configuration = &externalConfig;
	dbUsers = configuration->DBFolder() + "/" + configuration->UsersDatabaseName();
	tempUsers = configuration->TempFolder() + "/" + configuration->UsersDatabaseName();
	dbActors = configuration->DBFolder() + "/" + configuration->ActorsDatabaseName();
	tempActors = configuration->TempFolder() + "/" + configuration->ActorsDatabaseName();
	tempUsersInternal = configuration->TempFolder() + "/" + configuration->LocalUserDB();
	tempActorsInternal = configuration->TempFolder() + "/" + configuration->LocalActorDB();
	std::filesystem::create_directories(std::filesystem::path(configuration->TempFolder()));

	tracker.init();
	this->loadLocalDatabases();
	if (!users.empty() || !actors.empty())
	{
		updatingDB = true;
	}
	else
	{
		this->schedule([this]()
					   {
							   LOG(info) << "Server Start Scheduled";
						   startServer(); },
					   500);
	}
	this->schedule([this]()
				   {
						   LOG(info) << "Schedule Status Update";
			this->updateStatus(configuration->ServerOffMessage(), 0, SleepyDiscord::Status::online, false);
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
	if(checkServerStatus()!=ServerStatus::READY)
	{
		this->schedule([this]()
					   { scheduleRestart(); },
					   configuration->AlertInterval());
		return;
	}

	if (numberOfPlayers == 0)
	{
		sendMessage(configuration->AlertChannelId(), configuration->AlertMessage());
		this->schedule([this]()
					   {
			stopServer();
			startServer();
			this->schedule([this]() {
				scheduleRestart();
				}, configuration->RestartInterval() - configuration->AlertInterval()); },
					   configuration->RestartInterval() - configuration->AlertInterval());
					   
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

void RABIDS::cleanLocalDatabases()
{
	std::filesystem::remove(configuration->LocalUserDB());
	std::filesystem::remove(configuration->LocalActorDB());
}

void RABIDS::loadLocalDatabases()
{
	std::ifstream inputUsers(configuration->LocalUserDB(), std::ios_base::in);
	std::string line;
	if (inputUsers.is_open())
	{
		while (std::getline(inputUsers, line))
		{
			json user = json::parse(line);
			users.push_back(user);
		}

		inputUsers.close();
	}

	std::ifstream inputActors(configuration->LocalActorDB(), std::ios_base::in);
	if (inputActors.is_open())
	{
		while (std::getline(inputActors, line))
		{
			json actor = json::parse(line);
			actors.push_back(actor);
		}

		inputActors.close();
	}
}

int RABIDS::createChild(std::vector<std::string> arguments)
{
	int pid = fork();
	if (pid == 0)
	{
		int length = (int)arguments.size();
		const char **argv = new const char *[length + 1];
		for (int j = 0; j < length; ++j) // copy args
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
		// We are in the child process, execute the command
		execv(arguments[0].c_str(), (char **)argv);

		// If execl returns, there was an error
		LOG(error) << "Exec error: " << errno << ", " << strerror(errno);

		// Exit child process
		exit(1);
	}
	else if (pid > 0)
	{
		signal(SIGCHLD, SIG_IGN);
		return pid;
		// The parent process, do whatever is needed
		// The parent process can even exit while the child process is running, since it's independent
	}
	else
	{
		// Error forking, still in parent process (there are no child process at this point)
		LOG(error) << "Fork error: " << errno << ", " << strerror(errno);
		return -1;
	}
}

std::string RABIDS::exec(std::string command)
{
	char buffer[128];
	std::string result = "";

	// Open pipe to file
	FILE *pipe = popen(command.c_str(), "r");
	if (!pipe)
	{
		return "popen failed!";
	}

	// read till end of process:
	while (!feof(pipe))
	{

		// use buffer to read and add to result
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}

	pclose(pipe);
	return result;
}

json RABIDS::GetUserToUpdate(std::string userId)
{
	updateLocalDBInstance();
	std::string line;
	json user;
	bool found = false;
	std::ifstream input(tempUsers);
	while (std::getline(input, line))
	{
		user = json::parse(line);
		if (user["passwordSalt"] == userId)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		user = json::parse(configuration->UserTemplate());
		const auto ch_set = charset();
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, ch_set.size() - 1);
		auto randchar = [ch_set, &dist, &rng]()
		{ return ch_set[dist(rng)]; };
		auto length = 16;
		user["_id"] = random_string(length, randchar);
		user["passwordSalt"] = userId;
	}

	return user;
}

void RABIDS::updateLocalDBInstance()
{
	std::filesystem::create_directories(std::filesystem::path(configuration->TempFolder()));
	std::filesystem::copy(dbActors, tempActors, std::filesystem::copy_options::overwrite_existing);
	std::filesystem::copy(dbUsers, tempUsers, std::filesystem::copy_options::overwrite_existing);
}

bool RABIDS::updateDB()
{
	LOG(info) << "updateDB";
	std::fstream file;
	file.open(tempUsers, std::ios::out | std::ios::app);
	if (!file.is_open())
	{
		LOG(info) << "Failed To Open Users.DB";
		return false;
	}

	for (json user : users)
	{
		file << user.dump();
	}

	users.clear();
	file.close();
	file.open(tempActors, std::ios::out | std::ios::app);
	if (!file.is_open())
	{
		LOG(info) << "Failed To Open Actors.DB";
		return false;
	}

	for (json actor : actors)
	{
		file << actor.dump();
	}

	actors.clear();
	file.close();
	cleanLocalDatabases();
	dbChanged = false;
	return true;
}

bool RABIDS::uploadDBUsersDb()
{
	try
	{
		std::filesystem::copy(tempUsers, dbUsers, std::filesystem::copy_options::overwrite_existing);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool RABIDS::uploadDBActorsDb()
{

	try
	{
		std::filesystem::copy(tempActors, dbActors, std::filesystem::copy_options::overwrite_existing);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

json RABIDS::GetActorToUpdate(json user)
{
	updateLocalDBInstance();
	std::string line;
	json actor;
	bool found = false;
	std::ifstream input(tempActors);
	while (std::getline(input, line))
	{
		actor = json::parse(line);
		if (actor["permission"].contains(user["_id"]))
		{
			found = true;
			break;
		}
	}
	input.close();

	if (!found)
	{
		actor = json::parse(configuration->ActorTemplate());
		const auto ch_set = charset();
		std::default_random_engine rng(std::random_device{}());
		std::uniform_int_distribution<> dist(0, ch_set.size() - 1);
		auto randchar = [ch_set, &dist, &rng]()
		{ return ch_set[dist(rng)]; };
		auto length = 16;
		actor["_id"] = random_string(length, randchar);
		std::string permission = "{\"default\": 0,\"";
		permission += user["_id"];
		permission += "\": 3}";
		actor["permission"] = json::parse(permission);
		actor["name"] = user["name"];
		actor["token"]["name"] = user["name"];
	}
	else
	{
		actor = json::parse("{\"status\":\"null\"}");
	}

	return actor;
}
