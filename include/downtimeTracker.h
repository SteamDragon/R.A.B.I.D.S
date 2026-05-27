#pragma once
#include "sqlite3.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <downtimeEntry.h>
#include <vector>

using Record = std::vector<std::string>;
using Records = std::vector<Record>;

class downtimeTracker
{
public:
    downtimeTracker() = default;
    ~downtimeTracker();
    void init();
    downTimeEntry GetDowntimeEntry(std::string_view actorName);
    void InsertEntry(std::string_view discordId, std::string_view userName, std::string_view actorName);
    void AddDowntimes(const std::vector<std::string>& notAddDowntimeList, int numberOfDowntimes = 1);
    void ReduceDowntimes(std::string_view discordId, int numberOfDays);
    void AddHealing(std::string_view discordId, int numberOfDaysToHeal, int cryo = 0);
    void ReduceHealing(std::string_view discordId, int numberOfDays);
    std::string FormTable();
    std::string GetDate();
    void UpdateDate(int daysToAdd);
    std::string GetLastHealList() const;
    std::string GetLastCryoList() const;

private:
    std::string LastHealList;
    std::string LastCryoList;
    sqlite3 *ppDB = nullptr;
    const std::string downtimeDB = "data/downtime.db";

    Records GetAll();
    Record GetRecord(std::string_view discordId);

    int exec(const std::string& sql, const std::vector<std::string>& params = {});
    Records query(const std::string& sql, const std::vector<std::string>& params = {});
};