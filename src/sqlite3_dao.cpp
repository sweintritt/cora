#include "sqlite3_dao.hpp"

#include <plog/Log.h>

Sqlite3Dao::Sqlite3Dao() {
    
}

Sqlite3Dao::~Sqlite3Dao() {
    close();
}

void Sqlite3Dao::open(const std::string& url) {
    file = url;

    LOG(plog::debug) << "opening file " << url;
    if (sqlite3_open_v2(file.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw "unable to open " + file + "': " + getError();
    }

    IF_PLOG(plog::debug) {
        if (sqlite3_trace_v2(db, SQLITE_TRACE_STMT, &logStatement, nullptr) != SQLITE_OK) {
            LOG(plog::warning) << "unable to set up sqlite trace";
        }
    }

    onOpen():
}

void Sqlite3Dao::close() {
    LOG(plog::debug) << "closing database " << file;

    if (db != nullptr) {
        const bool result = onClose();

        if (result) {
            db = nullptr;
        } else {
            throw "error closing database: " + getError();
        }
    } else {
        LOG(plog::info) << "file already closed";
    }
}

void Sqlite3Dao::beginTransaction() {
    char* errorMessage;
    const std::string sql = "BEGIN TRANSACTION;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

void Sqlite3Dao::commit() {
    char* errorMessage;
    const std::string sql = "COMMIT;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

void Sqlite3Dao::rollback() {
    char* errorMessage;
    const std::string sql = "ROLLBACK;";
    if (sqlite3_exec(db, sql.c_str(), nullptr, 0, &errorMessage) != SQLITE_OK) {
        throw "unable to begin transaction: " + std::string{errorMessage};
    }
}

std::string Sqlite3Dao::getError() {
    const int errorCode = sqlite3_errcode(db);
    return std::string{ sqlite3_errstr(errorCode) };
}

void Sqlite3Dao::prepare(sqlite3_stmt** prepared, const std::string& stmnt) {
    if (sqlite3_prepare_v2(db, stmnt.c_str(), -1, prepared, nullptr) != SQLITE_OK) {
        throw "unable to prepare statement '" + stmnt + "': " + getError();
    }
}

int logStatement(unsigned int t, void* c, void* p, void* x) {
    if (t == SQLITE_TRACE_STMT) {
        LOG(plog::debug) << "sqlite stmt:'" << sqlite3_expanded_sql((sqlite3_stmt*) p) << "'";
    }

    return 0;
}
