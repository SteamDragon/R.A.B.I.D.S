#include "downtimeTracker.h"
#include "utils.h"
#include "VariadicTable.h"

static const int COL_DISCORD_ID = 1;
static const int COL_USER_NAME  = 2;
static const int COL_ACTOR_NAME = 3;
static const int COL_DOWNTIME   = 4;
static const int COL_HEALING    = 5;
static const int COL_CRYO       = 6;

int downtimeTracker::exec(const std::string& sql, const std::vector<std::string>& params)
{
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(ppDB, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(ppDB) << "\n  SQL: " << sql << "\n";
        return rc;
    }

    for (size_t i = 0; i < params.size(); i++)
    {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_OK)
    {
        std::cerr << "SQL exec error: " << sqlite3_errmsg(ppDB) << "\n  SQL: " << sql << "\n";
    }
    return rc;
}

Records downtimeTracker::query(const std::string& sql, const std::vector<std::string>& params)
{
    Records records;
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(ppDB, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL prepare error: " << sqlite3_errmsg(ppDB) << "\n  SQL: " << sql << "\n";
        return records;
    }

    for (size_t i = 0; i < params.size(); i++)
    {
        sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Record row;
        int colCount = sqlite3_column_count(stmt);
        for (int i = 0; i < colCount; i++)
        {
            const char* val = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.emplace_back(val ? val : "");
        }
        records.push_back(std::move(row));
    }

    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        std::cerr << "SQL query error: " << sqlite3_errmsg(ppDB) << "\n  SQL: " << sql << "\n";
    }
    return records;
}

void downtimeTracker::init()
{
    sqlite3_open(downtimeDB.c_str(), &ppDB);
    exec("CREATE TABLE IF NOT EXISTS downtime("
         "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
         "DiscordID           TEXT  DEFAULT \"\","
         "UserName           TEXT  DEFAULT \"\","
         "ActorName            TEXT DEFAULT \"\","
         "DowntimeDays        INT DEFAULT 0,"
         "Healing        INT DEFAULT 0,"
         "Cryo         INT DEFAULT 0);");

    exec("CREATE TABLE IF NOT EXISTS time("
         "data           TEXT  DEFAULT \"\");");

    if (query("select * from time;").empty())
    {
        exec("INSERT INTO time (data) VALUES('2045-Jan-01');");
    }
}

void downtimeTracker::UpdateDate(int daysToAdd)
{
    auto dateString = GetDate();
    boost::gregorian::date date(boost::gregorian::from_simple_string(dateString));
    boost::gregorian::date_duration dd(daysToAdd);
    exec("UPDATE time set data = ?;", {boost::gregorian::to_simple_string(date + dd)});
}

std::string downtimeTracker::GetDate()
{
    auto records = query("select * from time;");
    if (records.empty() || records.front().empty())
        return "2045-Jan-01";
    return records.front().front();
}

std::string downtimeTracker::GetLastHealList() const
{
    return LastHealList;
}

std::string downtimeTracker::GetLastCryoList() const
{
    return LastCryoList;
}

downtimeTracker::~downtimeTracker()
{
    if (ppDB)
        sqlite3_close(ppDB);
}

downTimeEntry downtimeTracker::GetDowntimeEntry(std::string_view actorName)
{
    Records records = query("SELECT * FROM downtime WHERE ActorName = ?;", {std::string(actorName)});
    if (records.empty())
    {
        std::cerr << "GetDowntimeEntry: no record found for actor " << actorName << "\n";
        return downTimeEntry{};
    }

    const auto& row = records.front();
    std::string discordId  = row[COL_DISCORD_ID];
    std::string userName   = row[COL_USER_NAME];
    std::string actName    = row[COL_ACTOR_NAME];
    int downtimeDays = std::stoi(row[COL_DOWNTIME]);
    int healing      = std::stoi(row[COL_HEALING]);
    int cryo         = std::stoi(row[COL_CRYO]);
    return downTimeEntry(userName, actName, downtimeDays, healing, cryo);
}

void downtimeTracker::AddDowntimes(const std::vector<std::string>& notAddDowntimeList, int numberOfDowntimes)
{
    LastHealList = "";
    LastCryoList = "";
    UpdateDate(numberOfDowntimes);
    auto excludeIds = notAddDowntimeList;
    auto all = GetAll();

    for (auto actor : all)
    {
        bool skip = std::find(excludeIds.begin(), excludeIds.end(), actor[COL_DISCORD_ID]) != excludeIds.end();
        if (std::stoi(actor[COL_HEALING]) > 0 && !skip)
        {
            for (auto i = 0; i < numberOfDowntimes; i++)
            {
                ReduceHealing(actor[COL_DISCORD_ID], 1);
            }

            if (std::stoi(GetRecord(actor[COL_DISCORD_ID])[COL_CRYO]) > 0)
            {
                excludeIds.push_back(actor[COL_DISCORD_ID]);
            }
        }
    }

    if (excludeIds.empty())
    {
        exec("UPDATE downtime SET DowntimeDays = DowntimeDays + ?;", {std::to_string(numberOfDowntimes)});
        return;
    }

    std::string sql = "UPDATE downtime SET DowntimeDays = DowntimeDays + ? WHERE DiscordID NOT IN (";
    std::vector<std::string> params;
    params.push_back(std::to_string(numberOfDowntimes));
    for (size_t i = 0; i < excludeIds.size(); i++)
    {
        if (i > 0) sql += ",";
        sql += "?";
        params.push_back(excludeIds[i]);
    }
    sql += ");";
    exec(sql, params);
}

void downtimeTracker::ReduceDowntimes(std::string_view discordId, int numberOfDays)
{
    exec("UPDATE downtime SET DowntimeDays = DowntimeDays - ? WHERE DiscordID = ?;",
         {std::to_string(numberOfDays), std::string(discordId)});
}

void downtimeTracker::ReduceHealing(std::string_view discordId, int numberOfDays)
{
    auto record = GetRecord(discordId);
    int cryo = std::stoi(record[COL_CRYO]);
    int number = std::stoi(record[COL_HEALING]) - numberOfDays;
    int baseCryo = cryo;
    if (cryo < numberOfDays)
    {
        cryo = 0;
    }
    else
    {
        cryo -= numberOfDays;
    }

    LastHealList = LastHealList.append(record[COL_ACTOR_NAME]).append(" : ").append(std::to_string(numberOfDays)).append(", ");
    if (baseCryo - cryo != 0)
    {
        LastCryoList = LastCryoList.append(record[COL_ACTOR_NAME]).append(" : ").append(std::to_string(baseCryo - cryo)).append(", ");
    }

    exec("UPDATE downtime SET Healing=?, Cryo=? WHERE DiscordID = ?;",
         {std::to_string(number), std::to_string(cryo), std::string(discordId)});
}

Records downtimeTracker::GetAll()
{
    return query("SELECT * FROM downtime;");
}

Record downtimeTracker::GetRecord(std::string_view discordId)
{
    auto records = query("SELECT * FROM downtime WHERE DiscordID = ?;", {std::string(discordId)});
    if (records.empty())
    {
        std::cerr << "GetRecord: no record for DiscordID " << discordId << "\n";
        return Record(7);
    }
    return records.front();
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
        bool cryo = std::stoi(rec[COL_CRYO]);
        std::string cryoString;
        int downtime = std::stoi(rec[COL_DOWNTIME]);
        std::string downtimeDays;
        int healing = std::stoi(rec[COL_HEALING]);
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

        vt.addRow(TranslitRusEng(rec[COL_USER_NAME]), TranslitRusEng(rec[COL_ACTOR_NAME]), downtimeDays, HealingDays, cryoString);
    }

    vt.print(ss);
    ss << "```";
    return ss.str();
}

void downtimeTracker::AddHealing(std::string_view discordId, int numberOfDaysToHeal, int cryo)
{
    auto record = GetRecord(discordId);
    int c = std::stoi(record[COL_CRYO]) + cryo;
    int number = std::stoi(record[COL_HEALING]) + numberOfDaysToHeal;
    exec("UPDATE downtime SET Healing=?, Cryo=? WHERE DiscordID = ?;",
         {std::to_string(number), std::to_string(c), std::string(discordId)});
}

void downtimeTracker::InsertEntry(std::string_view discordId, std::string_view userName, std::string_view actorName)
{
    exec("INSERT INTO downtime (DiscordID, UserName, ActorName) VALUES (?, ?, ?);",
         {std::string(discordId), std::string(userName), std::string(actorName)});
}

downTimeEntry::downTimeEntry(std::string_view userName, std::string_view actorName, int downtimeDays, int healing, int cryo) : UserName(userName), ActorName(actorName), DowntimeDays(downtimeDays), Healing(healing), Cryo(cryo) {}
