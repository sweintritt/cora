#include "sqlite_stations_dao.hpp"

#include <stdexcept>
#include <functional>
#include <sstream>

#include <plog/Log.h>

const std::string CREATE_TABLE_STATIONS_SQL = "CREATE TABLE IF NOT EXISTS stations ( "
        "id_hash INTEGER NOT NULL UNIQUE, "
        "name TEXT NOT NULL, "
        "author INTEGER NOT NULL, "
        "genre TEXT NOT NULL, "
        "country TEXT NOT NULL, "
        "language TEXT NOT NULL, "
        "description TEXT, "
        "urls TEXT NOT NULL);";
const std::string GET_STATION_SQL = "SELECT rowid, * FROM stations WHERE rowid = ?;";
const std::string INSERT_STATION_SQL = "INSERT INTO stations (id_hash, author, name, genre, country, language, description, urls) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

int logStatement(unsigned int t, void* c, void* p, void* x);

SqliteStationsDao::SqliteStationsDao() : version(1), db(nullptr) {

}

SqliteStationsDao::~SqliteStationsDao() {
    close();
}

SqliteStationsDao::SqliteStationsDao(const SqliteStationsDao& other) {
    throw std::runtime_error{ "SqliteStationsDao cannot be copied" };
}

void SqliteStationsDao::open(const std::string& url) {
    file = url;
    LOG(plog::info) << "creating file";
    if (sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw "unable to open " + file + "': " + getError();
    }

    LOG(plog::debug) << "creating table stations";
    char* errorMessage;
    if (sqlite3_exec(db, CREATE_TABLE_STATIONS_SQL.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to create table stations: " + getError();
    }

    IF_PLOG(plog::debug) {
        if (sqlite3_trace_v2(db, SQLITE_TRACE_STMT, &logStatement, nullptr) != SQLITE_OK) {
            LOG(plog::warning) << "unable to set up sqlite trace";
        }
    }

    LOG(plog::debug) << "preparing statements";
    prepare(&getStationStmnt, GET_STATION_SQL);
    prepare(&insertStationStmnt, INSERT_STATION_SQL);
}

void SqliteStationsDao::close() {
    LOG(plog::debug) << "closing database " << file;

    if (db != nullptr) {
        bool result = true;
        result &= sqlite3_finalize(getStationStmnt) == SQLITE_OK;
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
    // TODO check if the station has an id an just needs to be updated
    // INSERT OR UPDATE
    LOG(plog::debug) << "saving station '" << station.getName() << "'";
    if (station.getIdHash() == 0) {
        station.setIdHash(calculateHash(station));
        LOG(plog::debug) << "generated id hash for '" << station.getName() << "': " << station.getIdHash();
    } else {
        throw "station must be updated: id:" + station.getId();
    }

    // TODO check if an existing hash has changed
    sqlite3_bind_int64(insertStationStmnt, 1, station.getIdHash());
    sqlite3_bind_int(insertStationStmnt, 2, station.getAuthor());
    sqlite3_bind_text(insertStationStmnt, 3, station.getName().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 4, station.getGenre().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 5, station.getCountry().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 6, station.getLanguage().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(insertStationStmnt, 7, station.getDescription().c_str(), -1, SQLITE_STATIC);
    const std::string serializedUrls = serializeUrls(station.getUrls());
    sqlite3_bind_text(insertStationStmnt, 8, serializedUrls.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(insertStationStmnt) != SQLITE_DONE) {
        const std::string error = getError();
        sqlite3_reset(insertStationStmnt);
        throw "unable to store station: " + error;
    }

    const int64_t rowid = sqlite3_last_insert_rowid(db);
    station.setId(rowid);
    sqlite3_reset(insertStationStmnt);
}

Station SqliteStationsDao::findById(const long id) {
    sqlite3_bind_int(getStationStmnt, 1, id);
    const int rc = sqlite3_step(getStationStmnt);
    Station station;

    if (rc == SQLITE_ROW) {
        const uint64_t rowid = sqlite3_column_int(getStationStmnt, 0);
        const uint64_t idHash = sqlite3_column_int64(getStationStmnt, 1);
        const std::string name{(const char*) sqlite3_column_text(getStationStmnt, 2)};
        const std::string genre{(const char*) sqlite3_column_text(getStationStmnt, 3)};
        const std::string country{(const char*) sqlite3_column_text(getStationStmnt, 5)};
        const std::string language{(const char*) sqlite3_column_text(getStationStmnt, 6)};
        const std::string description{(const char*) sqlite3_column_text(getStationStmnt, 7)};
        const std::string urls{(const char*) sqlite3_column_text(getStationStmnt, 8)};
        station.setId(rowid);
        station.setIdHash(idHash);
        station.setName(name);
        station.setGenre(genre);
        station.setCountry(country);
        station.setLanguage(language);
        station.setDescription(description);
        for (auto& url : deserializeUrls(urls)) {
           station.addUrl(url);
        }
    } else if (rc == SQLITE_ERROR) {
        throw "unable to get station " + std::to_string(id) + ": " + getError();
    }

    sqlite3_reset(getStationStmnt);
    return station;
}

Station SqliteStationsDao::getRandom() {
    throw std::runtime_error("getRandom not implemented");
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

std::vector<Station> SqliteStationsDao::find(const std::string& name, const std::string& genre, const std::string& language, const std::string& country) {
    throw std::runtime_error("find not implemented");
}

std::vector<std::string> SqliteStationsDao::getGenres() {
    throw std::runtime_error("getGenres not implemented");
}

std::vector<std::string> SqliteStationsDao::getCountries() {
    throw std::runtime_error("getCountries not implemented");
}

std::vector<std::string> SqliteStationsDao::getLanguages() {
    throw std::runtime_error("ngetLanguages ot implemented");
}

std::string SqliteStationsDao::getError() {
    const int errorCode = sqlite3_errcode(db);
    return std::string{ sqlite3_errstr(errorCode) };
}

void SqliteStationsDao::prepare(sqlite3_stmt** prepared, const std::string stmnt) {
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
            const std::string url = value.substr(start, pos - 1);
            urls.push_back(url);
        }

        pos++;
    }

    return urls;
}

long SqliteStationsDao::calculateHash(const Station& station) {
    static std::hash<std::string> hash;
    return hash(station.getName()) + hash(station.getGenre()) + hash(station.getLanguage()) + hash(station.getCountry());
}

int logStatement(unsigned int t, void* c, void* p, void* x) {
    if (t == SQLITE_TRACE_STMT) {
        LOG(plog::debug) << "sqlite stmt:'" << sqlite3_expanded_sql((sqlite3_stmt*) p) << "'";
    }

    return 0;
}
