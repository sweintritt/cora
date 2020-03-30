#include "sqlite_stations_dao.hpp"

#include <stdexcept>
#include <iostream>


SqliteStationsDao::~SqliteStationsDao() {

}

void SqliteStationsDao::open(const std::string& url) {
    if (sqlite3_open_v2(url.c_str(), &db, SQLITE_OPEN_READWRITE, nullptr) != SQLITE_OK) {
        throw getError();
    }
}

void SqliteStationsDao::create(const std::string& url) {
    if (sqlite3_open_v2(url.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw getError();
    }
}

void SqliteStationsDao::close() {
    if (sqlite3_close_v2(db) != SQLITE_OK) {
        throw getError();
    }
}

void SqliteStationsDao::save(const Station& station) {
    throw std::runtime_error("not implemented");
}

Station SqliteStationsDao::get(const long id) {
    throw std::runtime_error("not implemented");
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
