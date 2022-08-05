#pragma once
#include <string>
class downTimeEntry
{
public:
    downTimeEntry() = default;
    downTimeEntry(std::string_view userName, std::string_view ActorName, int downtimeDays, int healing, int cryo);
    std::string UserName;
    std::string ActorName;
    std::string LastHealList;
    std::string LastCryoList;
    int DowntimeDays;
    int Healing;
    int Cryo;
};
