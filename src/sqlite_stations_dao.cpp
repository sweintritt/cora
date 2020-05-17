#include "sqlite_stations_dao.hpp"

#include <stdexcept>

#include "easylogging++.h"

const std::string CREATE_TABLE_STATIONS_SQL = "CREATE TABLE IF NOT EXISTS stations ( "
        "id_hash INTEGER PRIMARY KEY NOT NULL UNIQUE, "
        "author INTEGER NOT NULL, "
        "genre TEXT NOT NULL, "
        "counrty TEXT NOT NULL, "
        "language TEXT NOT NULL, "
        "description TEXT, "
        "urls TEXT NOT NULL) WITHOUT ROWID;";
const std::string GET_STATION_SQL = "SELECT * FROM stations WHERE id_hash = ?";


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
    DLOG(INFO) << "creating file";
    if (sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw "unable to open " + file + "': " + getError();
    }

    DLOG(INFO) << "creating table stations";
    char* errorMessage;
    if (sqlite3_exec(db, CREATE_TABLE_STATIONS_SQL.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to create table stations: " + getError();
    }

    DLOG(INFO) << "preparing statements";
    prepare(&getStationStmnt, GET_STATION_SQL);
}

void SqliteStationsDao::close() {
    DLOG(INFO) << "closing database " << file;

    if (db != nullptr) {
        bool result = true;
        result &= sqlite3_finalize(getStationStmnt) == SQLITE_OK;
        result &= sqlite3_close_v2(db) == SQLITE_OK;

        if (result) {
            db = nullptr;
        } else {
            throw getError();
        }
    } else {
        DLOG(INFO) << "file already closed";
    }
}

void SqliteStationsDao::save(const Station& station) {
    throw std::runtime_error("not implemented");

}

Station SqliteStationsDao::get(const long id) {
    sqlite3_bind_int(getStationStmnt, 1, id);
    const int rc = sqlite3_step(getStationStmnt);
    Station station;

    if (rc == SQLITE_ROW) {
        const long idHash = sqlite3_column_int(getStationStmnt, 0);
        const std::string name{(const char*) sqlite3_column_text(getStationStmnt, 1)};
        const std::string genre{(const char*) sqlite3_column_text(getStationStmnt, 2)};
        const std::string country{(const char*) sqlite3_column_text(getStationStmnt, 3)};
        const std::string language{(const char*) sqlite3_column_text(getStationStmnt, 4)};
        const std::string description{(const char*) sqlite3_column_text(getStationStmnt, 5)};
        const std::string urls{(const char*) sqlite3_column_text(getStationStmnt, 6)};
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
    throw std::runtime_error("not implemented");
}

int SqliteStationsDao::getVersion() {
    throw std::runtime_error("not implemented");
}

void SqliteStationsDao::create() {
    throw std::runtime_error("not implemented");
}

void SqliteStationsDao::upgrade(const int oldVersion, const int newVersion) {
    throw std::runtime_error("not implemented");
}

std::vector<Station> SqliteStationsDao::find(const std::string& name, const std::string& genre, const std::string& language, const std::string& country) {
    throw std::runtime_error("not implemented");
}

std::vector<std::string> SqliteStationsDao::getGenres() {
    throw std::runtime_error("not implemented");
}

std::vector<std::string> SqliteStationsDao::getCountries() {
    throw std::runtime_error("not implemented");
}

std::vector<std::string> SqliteStationsDao::getLanguages() {
    throw std::runtime_error("not implemented");
}

std::string SqliteStationsDao::getError() {
    const int errorCode = sqlite3_errcode(db);
    return std::string{ sqlite3_errstr(errorCode) };
}

void SqliteStationsDao::prepare(sqlite3_stmt** prepared, const std::string stmnt) {
    if (sqlite3_prepare_v2(db, stmnt.c_str(), -1, prepared, nullptr) != SQLITE_OK) {
        throw "unable to prepare statement: " + getError();
    }
}

std::string SqliteStationsDao::serializeUrls(const std::vector<std::string>& urls) {
    throw std::runtime_error("not implemented");
}

std::vector<std::string> SqliteStationsDao::deserializeUrls(const std::string& urls) {
    throw std::runtime_error("not implemented");
}

