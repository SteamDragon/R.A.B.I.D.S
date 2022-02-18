#pragma once

#include <fmt/format.h>
#include <config.h>
#include <downtimeTracker.h>
#include <sleepy_discord/sleepy_discord.h>

using json = nlohmann::json;
#define PIPE_READ 0
#define PIPE_WRITE 1
enum ServerStatus
{
	OFF = 0,
	READY = 1,
	DBUPDATE = 2
};

typedef std::vector<char> char_array;
class RABIDS : public SleepyDiscord::DiscordClient
{
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override;
	void startClient(config externalConfig);
	void onInteraction(SleepyDiscord::Interaction interaction) override;
	void onReady(SleepyDiscord::Ready ready) override;
	void onFirstConnect();
	std::string Register(SleepyDiscord::User user, std::string password);

private:
	config *configuration;
	bool statusUpdated = false;
	bool updatingDB = false;
	bool dbChanged = false;
	void scheduleStatusUpdate();
	ServerStatus checkServerStatus();
	std::string generalName = "";
	std::vector<json> users;
	std::vector<json> actors;
	int numberOfPlayers = 0;
	int pid = 0;
	int counter = 0;
	bool startServer();
	bool stopServer();
	std::string exec(std::string command);
	int createChild(std::vector<std::string> arguments);

	void updateLocalDBInstance();
	json GetUserToUpdate(std::string userId);
	json GetActorToUpdate(json user);
	bool updateDB();
	bool uploadDBUsersDb();
	bool uploadDBActorsDb();
	void loadLocalDatabases();
	void cleanLocalDatabases();
	void scheduleRestart();
	void SendErrorMessage();
	
	std::string dbUsers;
	std::string tempUsers;
	std::string dbActors;
	std::string tempActors;
	std::string tempUsersInternal;
	std::string tempActorsInternal;

	downtimeTracker tracker;
	std::string downtimeChannel = "938229440004116491";
	std::string downtimeMessage = "938512392332120106";
	std::string prevErrorMsg;
};

static std::string errorMessage = "";