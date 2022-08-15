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
setRestartTimeoutFailedMessage(messageTemplate.setRestartTimeoutFailedMessage),
botVersionMessage(messageTemplate.botVersionString)
{    
}

std::string Messages::AlertMessage() const
{
    return alertMessage;
}

std::string Messages::ServerOffMessage() const
{
    return serverOffMessage;
}

std::string Messages::DbUpdateMessage() const
{
    return dbUpdateMessage;
}

std::string Messages::ServerStartedMessage() const
{
    return serverStartedMessage;
}

std::string Messages::ServerFailedToStartMessage() const
{
    return serverFailedToStartMessage;
}

std::string Messages::ServerStoppedMessage() const
{
    return serverStoppedMessage;
}

std::string Messages::ServerFailedToStopMessage() const
{
    return serverFailedToStopMessage;
}

std::string Messages::ReadyMessage() const
{
    return readyMessage;
}

std::string Messages::HelpMessage() const
{
    return helpMessage;
}

std::string Messages::SetRestartTimeoutFailedMessage() const
{
    return setRestartTimeoutFailedMessage;
}


std::string Messages::BotVersionString() const
{
    return botVersionMessage;
}


std::string Messages::SucceedRegistrationString() const
{
    return succeedRegistrationString;
}

std::string Messages::SetRestartTimeoutFailedMessage() const
{
    return setRestartTimeoutFailedMessage;
}
