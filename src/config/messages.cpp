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

const std::string Messages::AlertMessage()
{
    return alertMessage;
}

const std::string Messages::ServerOffMessage()
{
    return serverOffMessage;
}

const std::string Messages::DbUpdateMessage()
{
    return dbUpdateMessage;
}

const std::string Messages::ServerStartedMessage()
{
    return serverStartedMessage;
}

const std::string Messages::ServerFailedToStartMessage()
{
    return serverFailedToStartMessage;
}

const std::string Messages::ServerStoppedMessage()
{
    return serverStoppedMessage;
}

const std::string Messages::ServerFailedToStopMessage()
{
    return serverFailedToStopMessage;
}

const std::string Messages::ReadyMessage()
{
    return readyMessage;
}

const std::string Messages::HelpMessage()
{
    return helpMessage;
}

const std::string Messages::SetRestartTimeoutFailedMessage()
{
    return setRestartTimeoutFailedMessage;
}
