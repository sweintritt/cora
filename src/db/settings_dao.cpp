#include "db/settings_dao.hpp"

#include <plog/Log.h>

const std::string CREATE_TABLE_SETTINGS_SQL = "CREATE TABLE IF NOT EXISTS settings ( "
        "key TEXT NOT NULL PRIMARY KEY, "
        "value TEXT NOT NULL);";
const std::string LOAD_SETTINGS_SQL = "SELECT value FROM settings WHERE key = ?;";
const std::string INSERT_SETTING_SQL = "INSERT OR IGNORE INTO settings (key, value) VALUES (?, ?);";

void SettingsDao::onOpen() {
    LOG(plog::debug) << "creating table settings";
    char* errorMessage;
    if (sqlite3_exec(db, CREATE_TABLE_SETTINGS_SQL.c_str(), nullptr, nullptr, &errorMessage) != SQLITE_OK) {
        throw "unable to create table stations: " + std::string{errorMessage};
    }

    LOG(plog::debug) << "preparing statements";
    prepare(&saveStmnt, INSERT_SETTING_SQL);
    prepare(&getStmnt, LOAD_SETTINGS_SQL);
}

bool SettingsDao::onClose() {
    bool result = true;
    result &= sqlite3_finalize(saveStmnt) == SQLITE_OK;
    result &= sqlite3_finalize(getStmnt) == SQLITE_OK;
    result &= sqlite3_close_v2(db) == SQLITE_OK;
    return result;
}

void SettingsDao::save(const std::string& key, const std::string& value) {
    LOG(plog::debug) << "saving setting '" << key << "'";

    sqlite3_bind_text(saveStmnt, 1, key.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(saveStmnt, 2, value.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(saveStmnt) != SQLITE_DONE) {
        const std::string error = getError();
        sqlite3_reset(saveStmnt);
        throw "unable to store setting: " + error;
    }

    sqlite3_reset(saveStmnt);
}

std::string SettingsDao::get(const std::string& key) {
    LOG(plog::debug) << "loading setting '" << key << "'";
    sqlite3_bind_text(getStmnt, 1, key.c_str(), -1, SQLITE_STATIC);
    const int rc = sqlite3_step(getStmnt);

    std::string value = "";
    if (rc == SQLITE_ROW) {
        value = std::string{(const char*) sqlite3_column_text(getStmnt, 0)};
    } else if (rc == SQLITE_ERROR) {
        sqlite3_reset(getStmnt);
        throw "unable to get setting " + key + ": " + getError();
    }

    sqlite3_reset(getStmnt);
    return value;
}

