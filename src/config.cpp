#include "config.h"

config::config(std::string configPath): _configPath(std::move(configPath)) {
	std::string config;
	std::string line;
	std::ifstream input(_configPath);
	while (std::getline(input, line))
	{
		config.append(line);
	}

	input.close();
	jsonConfig = json::parse(config);
	messagesTemplate template_;
	LOG(info) << "Configuration storage load";
	restartInterval = jsonConfig["RestartInterval"];
	alertInterval = jsonConfig["AlertInterval"];
	template_.botVersionString = jsonConfig["BotVersionString"];
	template_.helpMessage = jsonConfig["HelpMessage"];
	template_.serverOffMessage = jsonConfig["ServerOffMessage"];
	template_.dbUpdateMessage = jsonConfig["DBUpdateMessage"];
	dBFolder = jsonConfig["DBFolder"];
	tempFolder = jsonConfig["TempFolder"];
	usersDatabaseName = jsonConfig["UsersDatabaseName"];
	actorsDatabaseName = jsonConfig["ActorsDatabaseName"];
	localUserDB = jsonConfig["LocalUserDB"];
	localActorDB = jsonConfig["LocalActorDB"];
	playerCountFile = jsonConfig["PlayerCountFile"];
	channelId = jsonConfig["ChannelId"];
	alertChannelId = jsonConfig["AlertChannelId"];
	timedRegistrationString = jsonConfig["TimedRegistrationString"];
	template_.succeedRegistrationString = jsonConfig["SucceedRegistrationString"];
	userTemplate = jsonConfig["UserTemplate"];
	token = jsonConfig["Token"];
	template_.alertMessage = jsonConfig["AlertMessage"];
	actorTemplate = jsonConfig["ActorTemplate"];
	template_.readyMessage = jsonConfig["ReadyMessage"];
	template_.serverStartedMessage = jsonConfig["ServerStartedMessage"];
	template_.serverFailedToStartMessage = jsonConfig["ServerFailedToStartMessage"];
	template_.serverStoppedMessage = jsonConfig["ServerStoppedMessage"];
	template_.serverFailedToStopMessage = jsonConfig["ServerFailedToStopMessage"];
	restartTimeoutTemplate = jsonConfig["RestartTimeoutTemplate"];
	template_.setRestartTimeoutFailedMessage = jsonConfig["SetRestartTimeoutFailedMessage"];
	template_.generalError = jsonConfig["GeneralError"];
	template_.downtimeError = jsonConfig["DowntimeError"];
	template_.readManualError = jsonConfig["ReadManualError"];
	applicationAndArguments = jsonConfig["ApplicationAndArguments"].get<std::vector<std::string>>();
	textMessages = Messages(template_);
	LOG(info) << "Configuration storage load: Done";
}

std::string config::getVersion() const
{
    return version;
}

std::string config::DBFolder() const
{
    return dBFolder;
}

std::string config::TempFolder() const
{
    return tempFolder;
}

std::string config::UsersDatabaseName() const
{
    return usersDatabaseName;
}

std::string config::ActorsDatabaseName() const
{
    return actorsDatabaseName;
}

std::string config::LocalUserDB() const
{
    return localUserDB;
}

std::string config::LocalActorDB() const
{
    return localActorDB;
}

std::string config::PlayerCountFile() const
{
    return playerCountFile;
}

std::string config::ChannelId() const
{
    return channelId;
}

std::string config::AlertChannelId() const
{
    return alertChannelId;
}

std::string config::TimedRegistrationString() const
{
    return timedRegistrationString;
}

std::string config::UserTemplate() const
{
    return userTemplate;
}

std::string config::Token() const
{
    return token;
}

std::string config::ActorTemplate() const
{
    return actorTemplate;
}

std::string config::RestartTimeoutTemplate() const
{
    return restartTimeoutTemplate;
}

std::string config::GetConfigPath() const
{
    return _configPath;
}

std::vector<std::string> config::ApplicationAndArguments() const
{
    return applicationAndArguments;
}

long long config::RestartInterval() const
{
    return restartInterval;
}

long long config::AlertInterval() const
{
    return alertInterval;
}

void config::SetRestartInterval(long long newRestartInterval)
{
    jsonConfig["RestartInterval"] = newRestartInterval;
    restartInterval = newRestartInterval;
	
    std::ofstream output(_configPath);
    output << std::setw(4) << jsonConfig << std::endl;
	output.close();
}


    
Messages config::GetTextMessages() const
{
	return textMessages;
}