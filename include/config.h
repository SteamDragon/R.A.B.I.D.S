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
    std::string FoundryUrl() const;
    std::string FoundryApiKey() const;
    std::string BridgeDir() const;
    std::string ChannelId() const;
    std::string AlertChannelId() const;
    std::string TimedRegistrationString() const;
    std::string Token() const;
    std::string RestartTimeoutTemplate() const;
    std::string GetConfigPath() const;
    std::string DowntimeChannelId() const;
    std::string DowntimeMessageId() const;

    std::vector<std::string> ApplicationAndArguments() const;

    Messages GetTextMessages()  const;

    long long RestartInterval() const;
    void SetRestartInterval(long long newRestartInterval);
    long long AlertInterval() const;

private:
    const std::string version = "0.3.0";
    json jsonConfig;
    std::string _configPath;
    std::string foundryUrl;
    std::string foundryApiKey;
    std::string bridgeDir;
    std::string channelId;
    std::string alertChannelId;
    std::string timedRegistrationString;
    std::string token;
    std::string restartTimeoutTemplate;
    std::string downtimeChannelId;
    std::string downtimeMessageId;

    std::vector<std::string> applicationAndArguments;
    long long restartInterval;
    long long alertInterval;
    Messages textMessages;
};
