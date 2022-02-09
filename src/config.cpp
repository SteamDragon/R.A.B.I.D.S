#include "config.h"

config::config(std::string configPath)
{
    std::string config = "";
    std::string line;
    _configPath = configPath;
    std::ifstream input(_configPath);
    while (std::getline(input, line))
    {
        config.append(line);
    }

    input.close();
    jsonConfig = json::parse(config);
    restartInterval = jsonConfig["RestartInterval"];
    alertInterval = jsonConfig["AlertInterval"];

    helpMessage = jsonConfig["HelpMessage"];
    serverOffMessage = jsonConfig["ServerOffMessage"];
    dbUpdateMessage = jsonConfig["DBUpdateMessage"];
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
    succeedRegistrationString = jsonConfig["SucceedRegistrationString"];
    userTemplate = jsonConfig["UserTemplate"];
    token = jsonConfig["Token"];
    alertMessage = jsonConfig["AlertMessage"];
    actorTemplate = jsonConfig["ActorTemplate"];
    readyMessage = jsonConfig["ReadyMessage"];
    serverStartedMessage = jsonConfig["ServerStartedMessage"];
    serverFailedToStartMessage = jsonConfig["ServerFailedToStartMessage"];
    serverStoppedMessage = jsonConfig["ServerStoppedMessage"];
    serverFailedToStopMessage = jsonConfig["ServerFailedToStopMessage"];
    restartTimeoutTemplate = jsonConfig["RestartTimeoutTemplate"];
    setRestartTimeoutFailedMessage = jsonConfig["SetRestartTimeoutFailedMessage"];
    generalError = jsonConfig["GeneralError"];
    downtimeError = jsonConfig["DowntimeError"];
    readManualError = jsonConfig["ReadManualError"];
    applicationAndArguments = jsonConfig["ApplicationAndArguments"].get<std::vector<std::string>>();
}

config::~config()
{
    std::ofstream output(_configPath);
    output << std::setw(4) << jsonConfig << std::endl;
}

config::config()
{
}

std::string config::HelpMessage()
{
    return helpMessage;
}

std::string config::ServerOffMessage()
{
    return serverOffMessage;
}

std::string config::DbUpdateMessage()
{
    return dbUpdateMessage;
}

std::string config::BotVersionString()
{
    return botVersionString;
}

std::string config::DBFolder()
{
    return dBFolder;
}

std::string config::TempFolder()
{
    return tempFolder;
}

std::string config::UsersDatabaseName()
{
    return usersDatabaseName;
}

std::string config::ActorsDatabaseName()
{
    return actorsDatabaseName;
}

std::string config::LocalUserDB()
{
    return localUserDB;
}

std::string config::LocalActorDB()
{
    return localActorDB;
}

std::string config::PlayerCountFile()
{
    return playerCountFile;
}

std::string config::ChannelId()
{
    return channelId;
}

std::string config::AlertChannelId()
{
    return alertChannelId;
}

std::string config::TimedRegistrationString()
{
    return timedRegistrationString;
}

std::string config::SucceedRegistrationString()
{
    return succeedRegistrationString;
}

std::string config::UserTemplate()
{
    return userTemplate;
}

std::string config::Token()
{
    return token;
}

std::string config::AlertMessage()
{
    return alertMessage;
}

std::string config::ActorTemplate()
{
    return actorTemplate;
}

std::string config::ReadyMessage()
{
    return readyMessage;
}

std::string config::ServerStartedMessage()
{
    return serverStartedMessage;
}

std::string config::ServerFailedToStartMessage()
{
    return serverFailedToStartMessage;
}

std::string config::ServerStoppedMessage()
{
    return serverStoppedMessage;
}

std::string config::ServerFailedToStopMessage()
{
    return serverFailedToStopMessage;
}

std::string config::RestartTimeoutTemplate()
{
    return restartTimeoutTemplate;
}

std::string config::SetRestartTimeoutFailedMessage()
{
    return setRestartTimeoutFailedMessage;
}

std::string config::GeneralError()
{
    return generalError;
}

std::string config::DowntimeError()
{
    return downtimeError;
}

std::string config::ReadManualError()
{
    return readManualError;
}

std::vector<std::string> config::ApplicationAndArguments()
{
    return applicationAndArguments;
}

long long config::RestartInterval()
{
    return restartInterval;
}

long long config::AlertInterval()
{
    return alertInterval;
}

void config::SetRestartInterval(long long newRestartInterval)
{
    jsonConfig["RestartInterval"] = newRestartInterval;
    restartInterval = newRestartInterval;
}