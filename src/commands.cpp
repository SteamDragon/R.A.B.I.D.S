#include "commands.h"

namespace Command
{
    MappedCommands& all()
    {
        static MappedCommands instance;
        return instance;
    }

    void addCommand(Command cmd)
    {
        all().try_emplace(cmd.name, cmd);
    }
}
