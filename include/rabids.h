#pragma once

#include <fmt/format.h>
#include <config.h>
#include <downtimeTracker.h>
#include <syncEngine.h>
#include <sleepy_discord/sleepy_discord.h>
#include <exception>

using json = nlohmann::json;
enum class PIPE {PIPE_READ, PIPE_WRITE};
enum class ServerStatus
{
	OFF = 0,
	READY = 1,
	DBUPDATE = 2
};

using char_array=std::vector<char>;
class RABIDS : public SleepyDiscord::DiscordClient
{
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override;
	void startClient(config& externalConfig);
	void onInteraction(SleepyDiscord::Interaction interaction) override;
	void onReady(SleepyDiscord::Ready ready) override;
	void onFirstConnect();
	std::string Register(SleepyDiscord::User user, const std::string& password);

private:
	config *configuration;
	SyncEngine* sync;
	bool updatingDB = false;
	void scheduleStatusUpdate();
	ServerStatus checkServerStatus();
	std::string generalName = "";
	int numberOfPlayers = 0;
	int pid = 0;
	int counter = 0;
	bool startServer();
	bool stopServer();
	std::string exec(std::string command);
	int createChild(std::vector<std::string> arguments);
	void scheduleRestart();
	void SendErrorMessage();

	downtimeTracker tracker;
	std::string prevErrorMsg;
};
