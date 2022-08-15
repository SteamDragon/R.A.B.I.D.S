#pragma once
#include <string>

struct messagesTemplate
{
    std::string alertMessage;
    std::string serverOffMessage;
    std::string dbUpdateMessage;
    std::string serverStartedMessage;
    std::string serverFailedToStartMessage;
    std::string serverStoppedMessage;
    std::string serverFailedToStopMessage;
    std::string readyMessage;
    std::string helpMessage;
    std::string setRestartTimeoutFailedMessage;
    std::string botVersionString;
};

class Messages
{
public:
    Messages() = default;
    explicit Messages(const messagesTemplate& messageTemplate);
	std::string AlertMessage() const;
	std::string BotVersionString() const;
    std::string ServerOffMessage() const;
    std::string DbUpdateMessage() const;
    std::string ServerStartedMessage() const;
    std::string ServerFailedToStartMessage() const;
    std::string ServerStoppedMessage() const;
    std::string ServerFailedToStopMessage() const;
    std::string ReadyMessage() const; 
    std::string HelpMessage() const;
    std::string SucceedRegistrationString() const;
    std::string SetRestartTimeoutFailedMessage() const;


private:
    std::string alertMessage;
    std::string serverOffMessage;
    std::string dbUpdateMessage;
    std::string serverStartedMessage;
    std::string serverFailedToStartMessage;
    std::string serverStoppedMessage;
    std::string serverFailedToStopMessage;
    std::string readyMessage;
    std::string helpMessage;
    std::string setRestartTimeoutFailedMessage;
    std::string botVersionMessage;
    std::string succeedRegistrationString;
};
