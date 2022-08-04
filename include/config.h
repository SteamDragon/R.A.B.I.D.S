#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"

#define LOG(level) BOOST_LOG_TRIVIAL(level)

using json = nlohmann::json;

class config
{
public:
    config(std::string configPath);
    config(config& config);
    config();
    ~config();
    const std::string Version = "0.2.3";
    std::string HelpMessage();
    std::string ServerOffMessage();
    std::string DbUpdateMessage();
    std::string BotVersionString();
    std::string DBFolder();
    std::string TempFolder();
    std::string UsersDatabaseName();
    std::string ActorsDatabaseName();
    std::string LocalUserDB();
    std::string LocalActorDB();
    std::string PlayerCountFile();
    std::string ChannelId();
    std::string AlertChannelId();
    std::string TimedRegistrationString();
    std::string SucceedRegistrationString();
    std::string UserTemplate();
    std::string Token();
    std::string AlertMessage();
    std::string ActorTemplate();
    std::string ReadyMessage();
    std::string ServerStartedMessage();
    std::string ServerFailedToStartMessage();
    std::string ServerStoppedMessage();
    std::string ServerFailedToStopMessage();
    std::string RestartTimeoutTemplate();
    std::string SetRestartTimeoutFailedMessage();
    std::string GeneralError();
    std::string DowntimeError();
    std::string ReadManualError();
    std::string GetConfigPath();

    std::vector<std::string> ApplicationAndArguments();

    long long RestartInterval();
    void SetRestartInterval(long long newRestartInterval);
    long long AlertInterval();

private:
    json jsonConfig;
    std::string _configPath;

    std::string helpMessage;
    std::string serverOffMessage;
    std::string dbUpdateMessage;
    std::string botVersionString;
    std::string dBFolder;
    std::string tempFolder;
    std::string usersDatabaseName;
    std::string actorsDatabaseName;
    std::string localUserDB;
    std::string localActorDB;
    std::string playerCountFile;
    std::string channelId;
    std::string alertChannelId;
    std::string timedRegistrationString;
    std::string succeedRegistrationString;
    std::string userTemplate;
    std::string token;
    std::string alertMessage;
    std::string actorTemplate;
    std::string readyMessage;
    std::string serverStartedMessage;
    std::string serverFailedToStartMessage;
    std::string serverStoppedMessage;
    std::string serverFailedToStopMessage;
    std::string restartTimeoutTemplate;
    std::string setRestartTimeoutFailedMessage;
    std::string generalError;
    std::string downtimeError;
    std::string readManualError;

    std::vector<std::string> applicationAndArguments;

    long long restartInterval;
    long long alertInterval;
};