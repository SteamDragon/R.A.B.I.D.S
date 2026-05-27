#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct FoundryUser {
    std::string id;
    std::string name;
    int role = 0;
    std::string character;
    std::string discordId;
};

struct FoundryActor {
    std::string id;
    std::string name;
    std::string type;
    std::string img;
    json system;
    json flags;
};

struct SyncCommand {
    std::string action;
    json data;
};

class SyncEngine {
public:
    SyncEngine(std::string bridgeDir, std::string foundryUrl, std::string apiKey);

    bool loadUsers(std::vector<FoundryUser>& outUsers);
    bool loadActors(std::vector<FoundryActor>& outActors);
    bool loadPlayerCount(int& outCount);

    bool enqueueRegistration(const std::string& discordId, const std::string& name, const std::string& password);
    bool enqueueUpdateUser(const std::string& id, const json& changes);
    bool enqueueUpdateActor(const std::string& id, const json& changes);

    bool isFoundryAvailable() const;

private:
    std::string bridgeDir;
    std::string foundryUrl;
    std::string apiKey;

    json readJson(const std::string& path) const;
    bool writeCommand(const SyncCommand& cmd) const;
    std::string makeCommandId() const;
};
