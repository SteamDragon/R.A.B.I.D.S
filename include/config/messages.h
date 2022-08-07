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
};

class Messages
{
public:
    Messages() = default;
    explicit Messages(const messagesTemplate& messageTemplate);
	std::string AlertMessage();
    std::string ServerOffMessage();
    std::string DbUpdateMessage();
    std::string ServerStartedMessage();
    std::string ServerFailedToStartMessage();
    std::string ServerStoppedMessage();
    std::string ServerFailedToStopMessage();
    std::string ReadyMessage(); 
    std::string HelpMessage();
    std::string SetRestartTimeoutFailedMessage();

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
};
