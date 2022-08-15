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
    std::string DBFolder() const;
    std::string TempFolder() const;
    std::string UsersDatabaseName() const;
    std::string ActorsDatabaseName() const;
    std::string LocalUserDB() const;
    std::string LocalActorDB() const;
    std::string PlayerCountFile() const;
    std::string ChannelId() const;
    std::string AlertChannelId() const;
    std::string TimedRegistrationString() const;
    std::string SucceedRegistrationString() const;
    std::string UserTemplate() const;
    std::string Token() const;
    std::string ActorTemplate() const;
    std::string RestartTimeoutTemplate() const;
    std::string GeneralError() const;
    std::string DowntimeError() const;
    std::string ReadManualError() const;
    std::string GetConfigPath() const;
    

    std::vector<std::string> ApplicationAndArguments() const;

    Messages GetTextMessages()  const;

    long long RestartInterval() const;
    void SetRestartInterval(long long newRestartInterval);
    long long AlertInterval() const;

private:
    const std::string version = "0.2.4";
    json jsonConfig;
    std::string _configPath;
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
    std::string userTemplate;
    std::string token;
    std::string actorTemplate;
    std::string restartTimeoutTemplate;
    std::string generalError;
    std::string downtimeError;
    std::vector<std::string> applicationAndArguments;
    long long restartInterval;
    long long alertInterval;    
    Messages textMessages;
};