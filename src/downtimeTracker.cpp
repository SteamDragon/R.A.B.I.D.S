#include "downtimeTracker.h"
#include "VariadicTable.h"

void downtimeTracker::init()
{
    sqlite3_open(downtimeDB.c_str(), &ppDB);
    std::string sql = "CREATE TABLE IF NOT EXISTS downtime("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "DiscordID           TEXT  DEFAULT \"\","
                      "UserName           TEXT  DEFAULT \"\","
                      "ActorName            TEXT DEFAULT \"\","
                      "DowntimeDays        INT DEFAULT 0,"
                      "Healing        INT DEFAULT 0,"
                      "Cryo         INT DEFAULT 0);";

    rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        exit(-1);
    }

    sql = "CREATE TABLE IF NOT EXISTS time("
          "data           TEXT  DEFAULT \"\");";

    rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        exit(-1);
    }
    
    sql = "select * from time;";
    if (select_stmt(sql.c_str()).size() == 0)
    {
        sql = "INSERT INTO time (data) VALUES('2045-Jan-01'); ";

        rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            std::cerr << "SQL error: " << zErrMsg;
            sqlite3_free(zErrMsg);
            exit(-1);
        }
    }
}

void downtimeTracker::UpdateDate(int daysToAdd)
{
    auto dateString = GetDate();
    boost::gregorian::date date(boost::gregorian::from_simple_string(dateString));
    boost::gregorian::date_duration dd(daysToAdd);
    auto sql = std::string("Update time set data = \"").append(boost::gregorian::to_simple_string(date + dd)).append("\";");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
        exit(-1);
    }
}

std::string downtimeTracker::GetDate()
{
    auto sql = std::string("select * from time;");
    return select_stmt(sql.c_str()).front().front();
}

downtimeTracker::~downtimeTracker()
{
    sqlite3_finalize(stmt);
    sqlite3_close(ppDB);
}

downTimeEntry downtimeTracker::GetDowntimeEntry(std::string const & actorName)
{
    auto sql = std::string("Select * from downtime where downtime.ActorName == \"").append(actorName).append("\";");
    Records records = select_stmt(sql.c_str());
    std::string username = records.front()[1];
    std::string _actorName = records.front()[2];
    int downtimeDays = std::stoi(records.front()[3]);
    int healing = std::stoi(records.front()[4]);
    bool cryo = std::stoi(records.front()[5]);
    return downTimeEntry(username, _actorName, downtimeDays, healing, cryo);
}

void downtimeTracker::AddDowntimes(std::vector<std::string> notAddDowntimeList, int numberOfDowntimes)
{
    LastHealList = "";
    LastCryoList = "";
    UpdateDate(numberOfDowntimes);
    auto all = GetAll();
    for (auto actor : all)
    {
        bool found = !(std::find(notAddDowntimeList.begin(), notAddDowntimeList.end(), actor[1]) != notAddDowntimeList.end());
        if (std::stoi(actor[5]) > 0 && found)
        {
            for (auto i = 0; i < numberOfDowntimes; i++)
            {
                ReduceHealing(actor[1], 1);
            }

            if (std::stoi(GetRecord(actor[1])[5]) > 0)
            {
                notAddDowntimeList.push_back(actor[1]);
            }
        }
    }
    auto commaSepara = [](std::string a, std::string const & b)
    {
        if (b == "")
        {
            return a;
        }

        return std::move(a) + "\" AND DiscordID <> \"" + b;
    };

    LOG(info) << std::accumulate(std::next(notAddDowntimeList.begin()), notAddDowntimeList.end(),
                                 notAddDowntimeList[0], // start with first element
                                 commaSepara);

    auto sql = std::string("Update downtime set DowntimeDays = DowntimeDays + ").append(std::to_string(numberOfDowntimes)).append(" where DiscordID <> \"").append(std::accumulate(std::next(notAddDowntimeList.begin()), notAddDowntimeList.end(),
                                                                                                                                                                                   notAddDowntimeList[0], // start with first element
                                                                                                                                                                                   commaSepara))
                   .append("\";");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}

void downtimeTracker::ReduceDowntimes(std::string const & discordId, int numberOfDays)
{
    auto sql = std::string("Update downtime set DowntimeDays = DowntimeDays - ").append(std::to_string(numberOfDays)).append(" where DiscordID  = \"").append(discordId).append("\";");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, nullptr, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}

void downtimeTracker::ReduceHealing(std::string const & discordId, int numberOfDays)
{
    auto record = GetRecord(discordId);
    int cryo = std::stoi(record[6]);
    int number = std::stoi(record[5]) - numberOfDays;
    int baseCryo = cryo;
    if (cryo < numberOfDays)
    {
        cryo = 0;
    }
    else
    {
        cryo -= numberOfDays;
    }

    LastHealList = LastHealList.append(record[3]).append(" : ").append(std::to_string(numberOfDays)).append(", ");
    if (baseCryo - cryo != 0)
    {
        LastCryoList = LastCryoList.append(record[3]).append(" : ").append(std::to_string(baseCryo - cryo)).append(", ");
    }

    auto sql = std::string("Update downtime set Healing=").append(std::to_string(number)).append(", Cryo=").append(std::to_string(cryo)).append(" where DiscordID  = \"").append(discordId).append("\";");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}

Records downtimeTracker::GetAll()
{
    auto sql = "select * from downtime";
    return select_stmt(sql);
}

Record downtimeTracker::GetRecord(std::string const & discordId)
{
    auto sql = std::string("select * from downtime  where DiscordID  = \"") + discordId + "\"";
    return select_stmt(sql.c_str()).front();
}

std::string downtimeTracker::FormTable()
{
    std::stringstream ss;
    ss << "```c"<<std::endl;
    ss << GetDate()<<std::endl;
    VariadicTable<std::string, std::string, std::string, std::string, std::string> vt({"Username", "Actorname", "Downtime Days", "Healing", "Cryo"});
    Records records = GetAll();
    for (Record rec : records)
    {
        bool cryo = std::stoi(rec[6]);
        std::string cryoString;
        int downtime = std::stoi(rec[4]);
        std::string downtimeDays;
        int healing = std::stoi(rec[5]);
        std::string HealingDays;
        if (cryo)
        {
            cryoString = "✅";
        }
        else
        {
            cryoString = "❌";
        }

        if (downtime == 0)
        {
            downtimeDays = "-";
        }
        else
        {
            downtimeDays = std::to_string(downtime);
        }

        if (healing == 0)
        {
            HealingDays = "-";
        }
        else
        {
            HealingDays = std::to_string(healing);
        }

        vt.addRow(TranslitRusEng(rec[2]), TranslitRusEng(rec[3]), downtimeDays, HealingDays, cryoString);
    }

    vt.print(ss);
    ss << "```";
    return ss.str();
}

void downtimeTracker::AddHealing(std::string const & discordId, int numberOfDaysToHeal, int cryo)
{
    auto record = GetRecord(discordId);    
    int c = std::stoi(record[6]) + cryo;
    int number = std::stoi(record[5]) + numberOfDaysToHeal;
    auto sql = std::string("Update downtime set Healing=").append(std::to_string(number)).append(", Cryo=").append(std::to_string(c)).append(" where DiscordID  = \"").append(discordId).append("\";");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, 0, &zErrMsg);    
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}

void downtimeTracker::InsertEntry(std::string const & discordId, std::string const & userName, std::string const & actorName)
{
    auto sql = std::string("INSERT into downtime (DiscordID, UserName,ActorName) VALUES (\"").append(discordId).append("\",\"").append(userName).append("\",\"").append(actorName).append("\");");
    rc = sqlite3_exec(ppDB, sql.c_str(), callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << zErrMsg;
        sqlite3_free(zErrMsg);
    }
}

downTimeEntry::downTimeEntry(std::string const & userName, std::string const & actorName, int downtimeDays, int healing, int cryo)
{
    UserName = userName;
    ActorName = actorName;
    DowntimeDays = downtimeDays;
    Healing = healing;
    Cryo = cryo;
}