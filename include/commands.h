#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <sleepy_discord/sleepy_discord.h>

class RABIDS;

namespace Command
{
    using Verb = std::function<void(RABIDS&, SleepyDiscord::Interaction&)>;
    struct Command
    {
        std::string name;
        Verb verb;
    };
    using MappedCommands = std::unordered_map<std::string, Command>;

    MappedCommands& all();
    void addCommand(Command command);
}
