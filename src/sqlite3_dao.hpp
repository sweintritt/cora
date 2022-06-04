#pragma once

#include <string>
#include <sqlite3.h>

class Sqlite3Dao {
    public:
        Sqlite3Dao();

        virtual ~Sqlite3Dao() = default;

        virtual void open(const std::string& url);

        virtual void close();

        virtual void beginTransaction();

        virtual void commit();

        virtual void rollback();

    protected:
        std::string file;
        sqlite3* db;

        virtual std::string getError();
        virtual void prepare(sqlite3_stmt** prepared, const std::string& stmnt);
        virtual void onOpen() = 0;
        virtual bool onClose() = 0;
};
