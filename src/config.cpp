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
    
    LOG(info) << "Configuration storage load";
    restartInterval = jsonConfig["RestartInterval"];
    LOG(info) << "RestartInterval: " << restartInterval;
    alertInterval = jsonConfig["AlertInterval"];
    LOG(info) << "AlertInterval: " << alertInterval;
    helpMessage = jsonConfig["HelpMessage"];
    LOG(info) << "HelpMessage: " << helpMessage;
    serverOffMessage = jsonConfig["ServerOffMessage"];
    LOG(info) << "ServerOffMessage: " << serverOffMessage;
    dbUpdateMessage = jsonConfig["DBUpdateMessage"];
    LOG(info) << "DBUpdateMessage: " << dbUpdateMessage;
    dBFolder = jsonConfig["DBFolder"];
    LOG(info) << "DBFolder: " << dBFolder;
    tempFolder = jsonConfig["TempFolder"];
    LOG(info) << "TempFolder: " << tempFolder;
    usersDatabaseName = jsonConfig["UsersDatabaseName"];
    LOG(info) << "UsersDatabaseName: " << usersDatabaseName;
    actorsDatabaseName = jsonConfig["ActorsDatabaseName"];
    LOG(info) << "ActorsDatabaseName: " << actorsDatabaseName;
    localUserDB = jsonConfig["LocalUserDB"];
    LOG(info) << "LocalUserDB: " << localUserDB;
    localActorDB = jsonConfig["LocalActorDB"];
    LOG(info) << "LocalActorDB: " << localActorDB;
    playerCountFile = jsonConfig["PlayerCountFile"];
    LOG(info) << "PlayerCountFile: " << playerCountFile;
    channelId = jsonConfig["ChannelId"];
    LOG(info) << "ChannelId: " << channelId;
    alertChannelId = jsonConfig["AlertChannelId"];
    LOG(info) << "AlertChannelId: " << alertChannelId;
    timedRegistrationString = jsonConfig["TimedRegistrationString"];
    LOG(info) << "TimedRegistrationString: " << timedRegistrationString;
    succeedRegistrationString = jsonConfig["SucceedRegistrationString"];
    LOG(info) << "SucceedRegistrationString: " << succeedRegistrationString;
    userTemplate = jsonConfig["UserTemplate"];
    token = jsonConfig["Token"];
    LOG(info) << "Token: " << token;
    alertMessage = jsonConfig["AlertMessage"];
    LOG(info) << "AlertMessage: " << alertMessage;
    actorTemplate = jsonConfig["ActorTemplate"];
    readyMessage = jsonConfig["ReadyMessage"];
    LOG(info) << "ReadyMessage: " << readyMessage;
    serverStartedMessage = jsonConfig["ServerStartedMessage"];
    LOG(info) << "ServerStartedMessage: " << serverStartedMessage;
    serverFailedToStartMessage = jsonConfig["ServerFailedToStartMessage"];
    LOG(info) << "ServerFailedToStartMessage: " << serverFailedToStartMessage;
    serverStoppedMessage = jsonConfig["ServerStoppedMessage"];
    LOG(info) << "ServerStoppedMessage: " << serverStoppedMessage;
    serverFailedToStopMessage = jsonConfig["ServerFailedToStopMessage"];
    LOG(info) << "ServerFailedToStopMessage: " << serverFailedToStopMessage;
    restartTimeoutTemplate = jsonConfig["RestartTimeoutTemplate"];
    LOG(info) << "RestartTimeoutTemplate: " << restartTimeoutTemplate;
    setRestartTimeoutFailedMessage = jsonConfig["SetRestartTimeoutFailedMessage"];
    LOG(info) << "SetRestartTimeoutFailedMessage: " << setRestartTimeoutFailedMessage;
    generalError = jsonConfig["GeneralError"];
    LOG(info) << "GeneralError: " << generalError;
    downtimeError = jsonConfig["DowntimeError"];
    LOG(info) << "DowntimeError: " << downtimeError;
    readManualError = jsonConfig["ReadManualError"];
    LOG(info) << "ReadManualError: " << readManualError;
    applicationAndArguments = jsonConfig["ApplicationAndArguments"].get<std::vector<std::string>>();
    LOG(info) << "Configuration storage load: Done";
}

config::~config()
{
    std::ofstream output(_configPath);
    output << std::setw(4) << jsonConfig << std::endl;
}

config::config() : config("config.cfg")
{}

config::config(config& others) : config(others.GetConfigPath())
{  
}

const std::string config::HelpMessage()
{
    return helpMessage;
}

const std::string config::ServerOffMessage()
{
    return serverOffMessage;
}

const std::string config::DbUpdateMessage()
{
    return dbUpdateMessage;
}

const std::string config::BotVersionString()
{
    return botVersionString;
}

const std::string config::DBFolder()
{
    return dBFolder;
}

const std::string config::TempFolder()
{
    return tempFolder;
}

const std::string config::UsersDatabaseName()
{
    return usersDatabaseName;
}

const std::string config::ActorsDatabaseName()
{
    return actorsDatabaseName;
}

const std::string config::LocalUserDB()
{
    return localUserDB;
}

const std::string config::LocalActorDB()
{
    return localActorDB;
}

const std::string config::PlayerCountFile()
{
    return playerCountFile;
}

const std::string config::ChannelId()
{
    return channelId;
}

const std::string config::AlertChannelId()
{
    return alertChannelId;
}

const std::string config::TimedRegistrationString()
{
    return timedRegistrationString;
}

const std::string config::SucceedRegistrationString()
{
    return succeedRegistrationString;
}

const std::string config::UserTemplate()
{
    return userTemplate;
}

const std::string config::Token()
{
    return token;
}

const std::string config::AlertMessage()
{
    return alertMessage;
}

const std::string config::ActorTemplate()
{
    return actorTemplate;
}

const std::string config::ReadyMessage()
{
    return readyMessage;
}

const std::string config::ServerStartedMessage()
{
    return serverStartedMessage;
}

const std::string config::ServerFailedToStartMessage()
{
    return serverFailedToStartMessage;
}

const std::string config::ServerStoppedMessage()
{
    return serverStoppedMessage;
}

const std::string config::ServerFailedToStopMessage()
{
    return serverFailedToStopMessage;
}

const std::string config::RestartTimeoutTemplate()
{
    return restartTimeoutTemplate;
}

const std::string config::SetRestartTimeoutFailedMessage()
{
    return setRestartTimeoutFailedMessage;
}

const std::string config::GetConfigPath()
{
    return _configPath;
}

const std::string config::GeneralError()
{
    return generalError;
}

const std::string config::DowntimeError()
{
    return downtimeError;
}

const std::string config::ReadManualError()
{
    return readManualError;
}

const std::vector<std::string> config::ApplicationAndArguments()
{
    return applicationAndArguments;
}

const long long config::RestartInterval()
{
    return restartInterval;
}

const long long config::AlertInterval()
{
    return alertInterval;
}

void config::SetRestartInterval(long long newRestartInterval)
{
    jsonConfig["RestartInterval"] = newRestartInterval;
    restartInterval = newRestartInterval;
}