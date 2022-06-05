#include "settings_dao.hpp"

#include <plog/Log.h>

const std::string CREATE_TABLE_SETTINGS_SQL = "CREATE TABLE IF NOT EXISTS settings ( "
        "name TEXT NOT NULL, "
        "value TEXT NOT NULL);";
const std::string LOAD_SETTINGS_SQL = "SELECT * FROM settings;";
const std::string INSERT_SETTING_SQL = "INSERT INTO settings (name, value) VALUES (?, ?);";

SettingsDao::SettingsDao() {
    insertSettingStmnt = nullptr;
    loadSettingsStmnt = nullptr;
}

void SettingsDao::onOpen() {
    LOG(plog::debug) << "creating table settings";
    char* errorMessage;
    if (sqlite3_exec(db, CREATE_TABLE_SETTINGS_SQL.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to create table stations: " + std::string{errorMessage};
    }

    LOG(plog::debug) << "preparing statements";
    prepare(&insertSettingStmnt, INSERT_SETTING_SQL);
    prepare(&loadSettingsStmnt, LOAD_SETTINGS_SQL);
}

bool SettingsDao::onClose() {
    bool result = true;
    result &= sqlite3_finalize(insertSettingStmnt) == SQLITE_OK;
    result &= sqlite3_finalize(loadSettingsStmnt) == SQLITE_OK;
    result &= sqlite3_close_v2(db) == SQLITE_OK;
    return result;
}

void SettingsDao::save(Settings& settings) {
    for (const auto& setting : settings.all()) {
        LOG(plog::debug) << "saving setting '" << setting.first << "'";

        sqlite3_bind_text(insertSettingStmnt, 1, setting.first.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(insertSettingStmnt, 2, setting.second.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(insertSettingStmnt) != SQLITE_DONE) {
            const std::string error = getError();
            sqlite3_reset(insertSettingStmnt);
            throw "unable to store setting: " + error;
        }

        sqlite3_reset(insertSettingStmnt);
    }    
}

Settings SettingsDao::load() {
    Settings settings;
    
    int rc = sqlite3_step(loadSettingsStmnt);

    while (rc == SQLITE_ROW) {
        const std::string name{(const char*) sqlite3_column_text(loadSettingsStmnt, 0)};
        const std::string value{(const char*) sqlite3_column_text(loadSettingsStmnt, 1)};
        settings.set(name, value);
        rc = sqlite3_step(loadSettingsStmnt);
    }

    if (rc == SQLITE_ERROR) {
        throw "error while loading settings: " + getError();
    }

    sqlite3_reset(loadSettingsStmnt);
    return settings;
}

