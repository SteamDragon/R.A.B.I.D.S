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

std::string const config::HelpMessage()
{
    return helpMessage;
}

std::string const config::ServerOffMessage()
{
    return serverOffMessage;
}

std::string config::DbUpdateMessage()
{
    return dbUpdateMessage;
}

std::string const config::BotVersionString()
{
    return botVersionString;
}

std::string const config::DBFolder()
{
    return dBFolder;
}

std::string const config::TempFolder()
{
    return tempFolder;
}

std::string const config::UsersDatabaseName()
{
    return usersDatabaseName;
}

std::string const config::ActorsDatabaseName()
{
    return actorsDatabaseName;
}

std::string const config::LocalUserDB()
{
    return localUserDB;
}

std::string const config::LocalActorDB()
{
    return localActorDB;
}

std::string const config::PlayerCountFile()
{
    return playerCountFile;
}

std::string const config::ChannelId()
{
    return channelId;
}

std::string const config::AlertChannelId()
{
    return alertChannelId;
}

std::string const config::TimedRegistrationString()
{
    return timedRegistrationString;
}

std::string const config::SucceedRegistrationString()
{
    return succeedRegistrationString;
}

std::string const config::UserTemplate()
{
    return userTemplate;
}

std::string const config::Token()
{
    return token;
}

std::string const config::AlertMessage()
{
    return alertMessage;
}

std::string const config::ActorTemplate()
{
    return actorTemplate;
}

std::string const config::ReadyMessage()
{
    return readyMessage;
}

std::string const config::ServerStartedMessage()
{
    return serverStartedMessage;
}

std::string const config::ServerFailedToStartMessage()
{
    return const serverFailedToStartMessage;
}

std::string config::ServerStoppedMessage()
{
    return const serverStoppedMessage;
}

std::string config::ServerFailedToStopMessage()
{
    return const serverFailedToStopMessage;
}

std::string const config::RestartTimeoutTemplate()
{
    return restartTimeoutTemplate;
}

std::string const config::SetRestartTimeoutFailedMessage()
{
    return setRestartTimeoutFailedMessage;
}

std::string const config::GetConfigPath()
{
    return _configPath;
}

std::string const config::GeneralError()
{
    return generalError;
}

std::string const config::DowntimeError()
{
    return downtimeError;
}

std::string const config::ReadManualError()
{
    return readManualError;
}

std::vector<std::string> const config::ApplicationAndArguments()
{
    return applicationAndArguments;
}

long long const config::RestartInterval()
{
    return restartInterval;
}

long long const config::AlertInterval()
{
    return alertInterval;
}

void config::SetRestartInterval(long long newRestartInterval)
{
    jsonConfig["RestartInterval"] = newRestartInterval;
    restartInterval = newRestartInterval;
}