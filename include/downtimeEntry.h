#pragma once
#include <string>
class downTimeEntry
{
public:
    downTimeEntry();
    downTimeEntry(std::string userName, std::string ActorName, int downtimeDays, int healing, int cryo);
    std::string UserName;
    std::string ActorName;
    std::string LastHealList;
    std::string LastCryoList;
    int DowntimeDays;
    int Healing;
    int Cryo;
};
