#pragma once
#include <fstream>
#include <nlohmann/json.hpp>
#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"
#include "config/messages.h"

#define LOG(level) BOOST_LOG_TRIVIAL(level)

using json = nlohmann::json;

class config
{
public:
    explicit config(std::string configPath);
    std::string getVersion() const;
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
    std::string ActorTemplate();
    std::string RestartTimeoutTemplate();
    std::string GeneralError();
    std::string DowntimeError();
    std::string ReadManualError();
    std::string GetConfigPath() const;
    

    std::vector<std::string> ApplicationAndArguments();

    Messages GetTextMessages()  const;

    long long RestartInterval() const;
    void SetRestartInterval(long long newRestartInterval);
    long long AlertInterval() const;

private:
    const std::string version = "0.2.4";
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
    std::string generalError;
    std::string downtimeError;
    std::string readManualError;

    std::vector<std::string> applicationAndArguments;

    long long restartInterval;
    long long alertInterval;
    
    Messages textMessages;
};