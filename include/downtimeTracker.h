#pragma once
#include "sqlite3.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <downtimeEntry.h>
#include <config.h>
#include <VariadicTable.h>
using Record = std::vector<std::string>;
using Records = std::vector<Record>;
static downTimeEntry data;
class downtimeTracker
{
public:
    downtimeTracker();
    ~downtimeTracker();
    void init();
    downTimeEntry GetDowntimeEntry(std::string actorName);
    void InsertEntry(std::string discordId, std::string userName, std::string actorName);
    void AddDowntimes(std::vector<std::string> notAddDowntimeList, int numberOfDowntimes = 1);
    void ReduceDowntimes(std::string actorName, int numberOfDays);
    void AddHealing(std::string discordId, int numberOfDaysToHeal, int cryo = 0);
    void ReduceHealing(std::string discordId, int numberOfDays);
    std::string FormTable();
    std::string GetDate();
    void UpdateDate(int daysToAdd);
    std::string LastHealList;
    std::string LastCryoList;

private:
    sqlite3 *ppDB;
    const std::string downtimeDB = "data/downtime.db";
    int rc;
    char *zErrMsg = 0;
    sqlite3_stmt *stmt;

    Records GetAll();
    Record GetRecord(std::string discordId);
    static int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names)
    {
        Records *records = static_cast<Records *>(p_data);
        try
        {
            records->emplace_back(p_fields, p_fields + num_fields);
        }
        catch (...)
        {
            // abort select on failure, don't let exception propogate thru sqlite3 call-stack
            return 1;
        }
        return 0;
    }

    Records select_stmt(const char *stmt)
    {
        Records records;
        try
        {
            char *errmsg;
            int ret = sqlite3_exec(ppDB, stmt, select_callback, &records, &errmsg);
            if (ret != SQLITE_OK)
            {
                std::cerr << "Error in select statement " << stmt << "[" << errmsg << "]\n";
            }
        }
        catch (...)
        {
            std::cerr << "select_stmt\n";
        }

        return records;
    }

    static int callback(void *NotUsed, int argc, char **argv, char **azColName)
    {
        return 0;
    }

    static std::string TranslitRusEng(std::string &str)
    {
        bool find = false;
        std::string rus[74] = {"??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
                               "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??",
                               "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??", "??"};

        std::string eng[74] = {"A", "a", "B", "b", "V", "v", "G", "g", "G", "g", "D", "d", "E", "e", "E", "E", "Zh", "zh", "Z", "z", "I", "i", "I", "I", "Yi", "yi", "J", "j", "K", "k",
                               "L", "l", "M", "m", "N", "n", "O", "o", "P", "p", "R", "r", "S", "s", "T", "t", "U", "u", "F", "f", "H", "h", "Ts", "ts", "ch", "ch", "Sh", "sh",
                               "Shh", "shh", "'", "'", "Yu", "yu", "Ya", "ya", "Y", "y", "", "", "Yo", "yo", "E", "e"};
        std::string ret;

        for (long unsigned int i = 0; i < str.length(); i++)
        {
            find = false;
            for (int j = 0; j < 74; j++)
            {
                if (str.substr(i, 2).compare(rus[j]) == 0)
                {
                    ret += eng[j];
                    find = true;
                    i++;
                    break;
                }
            }

            if (!find)
                ret += str.substr(i, 1);
        }

        return ret;
    }
};

static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                    { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch)
                         { return !std::isspace(ch); })
                .base(),
            s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}