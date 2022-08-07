#pragma once
#include "sqlite3.h"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <downtimeEntry.h>
#include <config.h>
#include <VariadicTable.h>
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
    void AddDowntimes(std::vector<std::string> notAddDowntimeList, int numberOfDowntimes = 1);
    void ReduceDowntimes(std::string_view actorName, int numberOfDays);
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
    sqlite3 *ppDB;
    const std::string downtimeDB = "data/downtime.db";
    int rc;
    char *zErrMsg = nullptr;
    sqlite3_stmt *stmt;

    Records GetAll();
    Record GetRecord(std::string_view discordId);
    static int select_callback(void *p_data, int num_fields, char **p_fields,[[maybe_unused]] char **p_col_names)
    {
        auto records = static_cast<Records *>(p_data);
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

    Records select_stmt(const char *stmt_)
    {
        Records records;
        try
        {
            char *errmsg;
            int ret = sqlite3_exec(ppDB, stmt_, select_callback, &records, &errmsg);
            if (ret != SQLITE_OK)
            {
                std::cerr << "Error in select statement " << stmt_ << "[" << errmsg << "]\n";
            }
        }
        catch (...)
        {
            std::cerr << "select_stmt\n";
        }

        return records;
    }

    static int callback([[maybe_unused]] void *NotUsed, [[maybe_unused]] int argc, [[maybe_unused]] char **argv,[[maybe_unused]] char **azColName)
    {
        return 0;
    }

    static std::string TranslitRusEng(std::string &str)
    {
        bool find = false;
        std::string rus[74] = {"А", "а", "Б", "б", "В", "в", "Г", "г", "Ґ", "ґ", "Д", "д", "Е", "е", "Є", "є", "Ж", "ж", "З", "з", "И", "и", "І", "і", "Ї", "ї", "Й", "й", "К", "к",
                               "Л", "л", "М", "м", "Н", "н", "О", "о", "П", "п", "Р", "р", "С", "с", "Т", "т", "У", "у", "Ф", "ф", "Х", "х", "Ц", "ц", "Ч", "ч", "Ш", "ш",
                               "Щ", "щ", "Ь", "ь", "Ю", "ю", "Я", "я", "Ы", "ы", "Ъ", "ъ", "Ё", "ё", "Э", "э"};

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