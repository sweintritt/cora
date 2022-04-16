#include "sqlite_stations_dao.hpp"

#include <stdexcept>
#include <functional>
#include <sstream>

#include <plog/Log.h>

const std::string CREATE_TABLE_STATIONS_SQL = "CREATE TABLE IF NOT EXISTS stations ( "
        "name TEXT NOT NULL, "
        "addedBy TEXT NOT NULL, "
        "genre TEXT NOT NULL, "
        "country TEXT NOT NULL, "
        "language TEXT NOT NULL, "
        "description TEXT, "
        "urls TEXT NOT NULL);";
const std::string FIND_STATION_BY_ID_SQL = "SELECT rowid, * FROM stations WHERE rowid = ?;";
const std::string FIND_STATION_SQL = "SELECT rowid, * FROM stations WHERE name LIKE ? AND genre LIKE ? AND country LIKE ? ;";
const std::string DELETE_BY_ADDED_BY_SQL = "DELETE FROM stations WHERE addedBy = ?;";
const std::string INSERT_STATION_SQL = "INSERT INTO stations "
        "(addedBy, name, genre, country, language, description, urls) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
const std::string GET_ALL_IDS_SQL = "SELECT rowid FROM stations;";
const std::string SELECT_RANDOM_STATION_SQL = "SELECT rowid, * FROM stations ORDER BY random() limit 1;";

int logStatement(unsigned int t, void* c, void* p, void* x);

SqliteStationsDao::SqliteStationsDao()
    : version(1)
    , db(nullptr)
    , insertStationStmnt(nullptr)
    , findStationByIdStmnt(nullptr)
    , findStationStmnt(nullptr)
    , deleteByAddedByStmnt(nullptr)
    , getAllIdsStmnt(nullptr)
    , getRandomStationStmnt(nullptr) {
}

SqliteStationsDao::~SqliteStationsDao() {
    close();
}

void SqliteStationsDao::open(const std::string& url) {
    file = url;
    LOG(plog::debug) << "opening file " << url;
    if (sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw "unable to open " + file + "': " + getError();
    }

    LOG(plog::debug) << "creating table stations";
    char* errorMessage;
    if (sqlite3_exec(db, CREATE_TABLE_STATIONS_SQL.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to create table stations: " + std::string{errorMessage};
    }

    IF_PLOG(plog::debug) {
        if (sqlite3_trace_v2(db, SQLITE_TRACE_STMT, &logStatement, nullptr) != SQLITE_OK) {
            LOG(plog::warning) << "unable to set up sqlite trace";
        }
    }

    LOG(plog::debug) << "preparing statements";
    prepare(&findStationByIdStmnt, FIND_STATION_BY_ID_SQL);
    prepare(&findStationStmnt, FIND_STATION_SQL);
    prepare(&deleteByAddedByStmnt, DELETE_BY_ADDED_BY_SQL);
    prepare(&insertStationStmnt, INSERT_STATION_SQL);
    prepare(&getAllIdsStmnt, GET_ALL_IDS_SQL);
    prepare(&getRandomStationStmnt, SELECT_RANDOM_STATION_SQL);
}

void SqliteStationsDao::close() {
    LOG(plog::debug) << "closing database " << file;

    if (db != nullptr) {
        bool result = true;
        result &= sqlite3_finalize(insertStationStmnt) == SQLITE_OK;
        result &= sqlite3_finalize(findStationByIdStmnt) == SQLITE_OK;
        result &= sqlite3_finalize(findStationStmnt) == SQLITE_OK;
        result &= sqlite3_finalize(deleteByAddedByStmnt) == SQLITE_OK; 
        result &= sqlite3_finalize(getAllIdsStmnt) == SQLITE_OK;
        result &= sqlite3_finalize(getRandomStationStmnt) == SQLITE_OK;
        result &= sqlite3_close_v2(db) == SQLITE_OK;

        if (result) {
            db = nullptr;
        } else {
            throw "error closing database: " + getError();
        }
    } else {
        LOG(plog::info) << "file already closed";
    }
}

void SqliteStationsDao::save(Station& station) {
    LOG(plog::debug) << "saving station '" << station.getName() << "'";

    sqlite3_bind_text(insertStationStmnt, 1, station.getAddedBy().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 2, station.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 3, station.getGenre().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 4, station.getCountry().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 5, station.getLanguage().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 6, station.getDescription().c_str(), -1, SQLITE_STATIC);
    const std::string serializedUrls = serializeUrls(station.getUrls());
    sqlite3_bind_text(insertStationStmnt, 7, serializedUrls.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(insertStationStmnt) != SQLITE_DONE) {
        const std::string error = getError();
        sqlite3_reset(insertStationStmnt);
        throw "unable to store station: " + error;
    }

    const int64_t rowid = sqlite3_last_insert_rowid(db);
    station.setId(rowid);
    sqlite3_reset(insertStationStmnt);
}

std::vector<long> SqliteStationsDao::find(const std::string& name, const std::string& genre, const std::string& country) {
    LOG(plog::debug) << "searching for station name:" << name << ", genre:" << genre << ", country:" << country;

    const std::string likeName = "%" + name + "%";
    const std::string likeGenre = "%" + genre + "%";
    const std::string likeCountry = "%" + country + "%";

    sqlite3_bind_text(findStationStmnt, 1, likeName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(findStationStmnt, 2, likeGenre.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(findStationStmnt, 3, likeCountry.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(findStationStmnt);
    std::vector<long> ids;

    while (rc == SQLITE_ROW) {
        ids.push_back(sqlite3_column_int64(findStationStmnt, 0));
        rc = sqlite3_step(findStationStmnt);
    }

    if (rc == SQLITE_ERROR) {
        throw "error while loading ids: " + getError();
    }

    sqlite3_reset(findStationStmnt);
    return ids;
}

std::shared_ptr<Station> SqliteStationsDao::findById(const long id) {
    sqlite3_bind_int(findStationByIdStmnt, 1, id);
    const int rc = sqlite3_step(findStationByIdStmnt);
    std::shared_ptr<Station> station = nullptr;

    if (rc == SQLITE_ROW) {
        station = std::make_shared<Station>(getStation(findStationByIdStmnt));
    } else if (rc == SQLITE_ERROR) {
        sqlite3_reset(findStationByIdStmnt);
        throw "unable to get station " + std::to_string(id) + ": " + getError();
    }

    sqlite3_reset(findStationByIdStmnt);
    return station;
}

void SqliteStationsDao::deleteAllAddedBy(const std::string& addedBy) {
    sqlite3_bind_text(deleteByAddedByStmnt, 1, addedBy.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(deleteByAddedByStmnt) != SQLITE_DONE) {
        const std::string error = getError();
        sqlite3_reset(deleteByAddedByStmnt);
        throw "unable to delete stations added by author:" + addedBy + ": " + error;
    }

    sqlite3_reset(deleteByAddedByStmnt);
}

std::shared_ptr<Station> SqliteStationsDao::getRandom() {
    const int rc = sqlite3_step(getRandomStationStmnt);
    std::shared_ptr<Station> station = nullptr;

    if (rc == SQLITE_ROW) {
        station = std::make_shared<Station>(getStation(getRandomStationStmnt));
    } else if (rc == SQLITE_ERROR) {
        sqlite3_reset(getRandomStationStmnt);
        throw "unable to get station : " + getError();
    }

    sqlite3_reset(getRandomStationStmnt);
    return station;
}

int SqliteStationsDao::getVersion() {
    throw std::runtime_error("getVersion not implemented");
}

void SqliteStationsDao::create() {
    throw std::runtime_error("create not implemented");
}

void SqliteStationsDao::upgrade(const int oldVersion, const int newVersion) {
    throw std::runtime_error("upgrade not implemented");
}

std::vector<long> SqliteStationsDao::getAllIds() {
    int rc = sqlite3_step(getAllIdsStmnt);
    std::vector<long> ids;

    while (rc == SQLITE_ROW) {
        ids.push_back(sqlite3_column_int64(getAllIdsStmnt, 0));
        rc = sqlite3_step(getAllIdsStmnt);
    }

    if (rc == SQLITE_ERROR) {
        throw "error while loading ids: " + getError();
    }

    sqlite3_reset(getAllIdsStmnt);
    return ids;
}

void SqliteStationsDao::beginTransaction() {
    char* errorMessage;
    const std::string sql = "BEGIN TRANSACTION;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

void SqliteStationsDao::commit() {
    char* errorMessage;
    const std::string sql = "COMMIT;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

void SqliteStationsDao::rollback() {
    char* errorMessage;
    const std::string sql = "ROLLBACK;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

std::string SqliteStationsDao::getError() {
    const int errorCode = sqlite3_errcode(db);
    return std::string{ sqlite3_errstr(errorCode) };
}

void SqliteStationsDao::prepare(sqlite3_stmt** prepared, const std::string& stmnt) {
    if (sqlite3_prepare_v2(db, stmnt.c_str(), -1, prepared, nullptr) != SQLITE_OK) {
        throw "unable to prepare statement '" + stmnt + "': " + getError();
    }
}

std::string SqliteStationsDao::serializeUrls(const std::vector<std::string>& urls) {
    std::stringstream stream;
    for (const auto& url : urls) {
        stream << "{" << url << "}";
    }
    return stream.str();
}

std::vector<std::string> SqliteStationsDao::deserializeUrls(const std::string& value) {
    std::vector<std::string> urls;

    size_t pos = 0;
    while (pos != std::string::npos && pos < value.size()) {
        if (value[pos] != '{') {
            throw "no valid serialized url string: '" + value + "'";
        }

        const size_t start = pos + 1;
        pos = value.find("}", start);

        if (pos != std::string::npos && pos < value.size()) {
            const size_t length = pos - start;
            const std::string url = value.substr(start, length);
            urls.push_back(url);
        }

        pos++;
    }

    return urls;
}

Station SqliteStationsDao::getStation(sqlite3_stmt* stmnt) {
    const uint64_t rowid = sqlite3_column_int64(stmnt, 0);
    const std::string name{(const char*) sqlite3_column_text(stmnt, 1)};
    const std::string addedBy{(const char*) sqlite3_column_text(stmnt, 2)};
    const std::string genre{(const char*) sqlite3_column_text(stmnt, 3)};
    const std::string country{(const char*) sqlite3_column_text(stmnt, 4)};
    const std::string language{(const char*) sqlite3_column_text(stmnt, 5)};
    const std::string description{(const char*) sqlite3_column_text(stmnt, 6)};
    const std::string urls{(const char*) sqlite3_column_text(stmnt, 7)};

    Station station;
    station.setId(rowid);
    station.setName(name);
    station.setAddedBy(addedBy);
    station.setGenre(genre);
    station.setCountry(country);
    station.setLanguage(language);
    station.setDescription(description);

    for (auto& url : deserializeUrls(urls)) {
        station.addUrl(url);
    }

    return station;
}

int logStatement(unsigned int t, void* c, void* p, void* x) {
    if (t == SQLITE_TRACE_STMT) {
        LOG(plog::debug) << "sqlite stmt:'" << sqlite3_expanded_sql((sqlite3_stmt*) p) << "'";
    }

    return 0;
}
