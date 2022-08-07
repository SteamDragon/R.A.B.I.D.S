#include "messages.h"

Messages::Messages(const messagesTemplate& messageTemplate) : alertMessage(messageTemplate.alertMessage),
serverOffMessage(messageTemplate.serverOffMessage),
dbUpdateMessage(messageTemplate.dbUpdateMessage),
serverStartedMessage(messageTemplate.serverStartedMessage),
serverFailedToStartMessage(messageTemplate.serverFailedToStartMessage),
serverStoppedMessage(messageTemplate.serverStoppedMessage),
serverFailedToStopMessage(messageTemplate.serverFailedToStopMessage),
readyMessage(messageTemplate.readyMessage),
helpMessage(messageTemplate.helpMessage),
setRestartTimeoutFailedMessage(messageTemplate.setRestartTimeoutFailedMessage)
{    
}

std::string Messages::AlertMessage()
{
    return alertMessage;
}

std::string Messages::ServerOffMessage()
{
    return serverOffMessage;
}

std::string Messages::DbUpdateMessage()
{
    return dbUpdateMessage;
}

std::string Messages::ServerStartedMessage()
{
    return serverStartedMessage;
}

std::string Messages::ServerFailedToStartMessage()
{
    return serverFailedToStartMessage;
}

std::string Messages::ServerStoppedMessage()
{
    return serverStoppedMessage;
}

std::string Messages::ServerFailedToStopMessage()
{
    return serverFailedToStopMessage;
}

std::string Messages::ReadyMessage()
{
    return readyMessage;
}

std::string Messages::HelpMessage()
{
    return helpMessage;
}

std::string Messages::SetRestartTimeoutFailedMessage()
{
    return setRestartTimeoutFailedMessage;
}
