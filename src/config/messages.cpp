#include "messages.h"

Messages::Messages()
{
}

Messages::Messages(messagesTemplate messageTemplate)
{
    alertMessage = template.alertMessage;
    serverOffMessage = template.serverOffMessage;
    dbUpdateMessage= template.dbUpdateMessage;
    serverStartedMessage = template.serverStartedMessage;
    serverFailedToStartMessage= template.serverFailedToStartMessage;
    serverStoppedMessage = template.serverStoppedMessage;
    serverFailedToStopMessage = template.serverFailedToStopMessage;
    readyMessage = template.readyMessage;
    helpMessage = template.helpMessage;
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
