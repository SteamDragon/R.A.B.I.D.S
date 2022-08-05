﻿#pragma once

#include <fmt/format.h>
#include <config.h>
#include <downtimeTracker.h>
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
	void startClient(config externalConfig);
	void onInteraction(SleepyDiscord::Interaction interaction) override;
	void onReady(SleepyDiscord::Ready ready) override;
	void onFirstConnect();
	std::string Register(SleepyDiscord::User user, std::string password);

private:
	config *configuration;
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