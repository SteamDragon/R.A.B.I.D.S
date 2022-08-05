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
    std::string const HelpMessage();
    std::string const ServerOffMessage();
    std::string const DbUpdateMessage();
    std::string const BotVersionString();
    std::string const DBFolder();
    std::string const TempFolder();
    std::string const UsersDatabaseName();
    std::string const ActorsDatabaseName();
    std::string const LocalUserDB();
    std::string const LocalActorDB();
    std::string const PlayerCountFile();
    std::string const ChannelId();
    std::string const AlertChannelId();
    std::string const TimedRegistrationString();
    std::string const SucceedRegistrationString();
    std::string const UserTemplate();
    std::string const Token();
    std::string const AlertMessage();
    std::string const ActorTemplate();
    std::string const ReadyMessage();
    std::string const ServerStartedMessage();
    std::string const ServerFailedToStartMessage();
    std::string const ServerStoppedMessage();
    std::string const ServerFailedToStopMessage();
    std::string const RestartTimeoutTemplate();
    std::string const GeneralError();
    std::string const DowntimeError();
    std::string const ReadManualError();
    std::string const GetConfigPath();
    std::string const SetRestartTimeoutFailedMessage();

    std::vector<std::string> const ApplicationAndArguments();

    long long const RestartInterval();
    void SetRestartInterval(long long newRestartInterval);
    long long const AlertInterval();

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