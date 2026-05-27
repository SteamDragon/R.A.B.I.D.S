#include "syncEngine.h"
#include <fstream>
#include <filesystem>
#include <random>
#include <sstream>
#include <curl/curl.h>

SyncEngine::SyncEngine(std::string bridgeDir, std::string foundryUrl, std::string apiKey)
    : bridgeDir(std::move(bridgeDir))
    , foundryUrl(std::move(foundryUrl))
    , apiKey(std::move(apiKey))
{
    std::filesystem::create_directories(this->bridgeDir);
    std::filesystem::create_directories(this->bridgeDir + "/commands/done");

    if (!this->apiKey.empty()) {
        json cfg = {{"apiKey", this->apiKey}};
        std::string tmp = this->bridgeDir + "/config.tmp";
        {
            std::ofstream f(tmp);
            f << cfg.dump();
        }
        std::error_code ec;
        std::filesystem::rename(tmp, this->bridgeDir + "/config.json", ec);
    }
}

json SyncEngine::readJson(const std::string& path) const
{
    std::ifstream file(path);
    if (!file.is_open())
        return nullptr;

    std::stringstream ss;
    ss << file.rdbuf();
    try {
        return json::parse(ss.str());
    } catch (...) {
        return nullptr;
    }
}

bool SyncEngine::loadUsers(std::vector<FoundryUser>& outUsers)
{
    auto data = readJson(bridgeDir + "/users.json");
    if (!data || !data.is_array())
        return false;

    outUsers.clear();
    for (const auto& u : data) {
        FoundryUser user;
        user.id = u.value("_id", "");
        user.name = u.value("name", "");
        user.role = u.value("role", 0);
        user.character = u.value("character", "");
        if (u.contains("flags") && u["flags"].contains("rabids") && u["flags"]["rabids"].contains("discordId"))
            user.discordId = u["flags"]["rabids"]["discordId"];
        outUsers.push_back(std::move(user));
    }
    return true;
}

bool SyncEngine::loadActors(std::vector<FoundryActor>& outActors)
{
    auto data = readJson(bridgeDir + "/actors.json");
    if (!data || !data.is_array())
        return false;

    outActors.clear();
    for (const auto& a : data) {
        FoundryActor actor;
        actor.id = a.value("_id", "");
        actor.name = a.value("name", "");
        actor.type = a.value("type", "");
        actor.img = a.value("img", "");
        if (a.contains("system"))
            actor.system = a["system"];
        if (a.contains("flags"))
            actor.flags = a["flags"];
        outActors.push_back(std::move(actor));
    }
    return true;
}

bool SyncEngine::loadPlayerCount(int& outCount)
{
    auto data = readJson(bridgeDir + "/players.json");
    if (!data || !data.is_object())
        return false;
    outCount = data.value("count", 0);
    return true;
}

std::string SyncEngine::makeCommandId() const
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 15);
    const char hex[] = "0123456789abcdef";
    std::string id(32, '0');
    for (auto& c : id)
        c = hex[dist(gen)];
    return id;
}

bool SyncEngine::writeCommand(const SyncCommand& cmd) const
{
    std::string id = makeCommandId();
    std::string path = bridgeDir + "/commands/" + id + ".json";
    std::string tmp = bridgeDir + "/commands/." + id + ".tmp";

    json payload = {{"action", cmd.action}, {"data", cmd.data}};
    if (!apiKey.empty())
        payload["apiKey"] = apiKey;
    {
        std::ofstream file(tmp);
        if (!file.is_open())
            return false;
        file << payload.dump();
        file.close();
    }

    std::error_code ec;
    std::filesystem::rename(tmp, path, ec);
    return !ec;
}

bool SyncEngine::enqueueRegistration(const std::string& discordId, const std::string& name, const std::string& password)
{
    return writeCommand({
        "registerUser",
        {{"discordId", discordId}, {"name", name}, {"password", password}}
    });
}

bool SyncEngine::enqueueUpdateUser(const std::string& id, const json& changes)
{
    return writeCommand({
        "updateUser",
        {{"id", id}, {"changes", changes}}
    });
}

bool SyncEngine::enqueueUpdateActor(const std::string& id, const json& changes)
{
    return writeCommand({
        "updateActor",
        {{"id", id}, {"changes", changes}}
    });
}

static size_t discardData(char*, size_t size, size_t nmemb, void*)
{
    return size * nmemb;
}

bool SyncEngine::isFoundryAvailable() const
{
    std::string url = foundryUrl + "/api";

    CURL* curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discardData);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        return false;
    }

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    return httpCode == 200 || httpCode == 401 || httpCode == 403;
}
