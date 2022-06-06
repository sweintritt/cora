#pragma once

#include <string>
#include <sqlite3.h>

class Dao {
    public:
        Dao() = default;
        virtual ~Dao() = default;
        void open(const std::string& url);
        void close();
        void beginTransaction();
        void commit();
        void rollback();

    protected:
        std::string file;
        sqlite3* db;

        std::string getError();
        void prepare(sqlite3_stmt** prepared, const std::string& stmnt);
        virtual void onOpen() = 0;
        virtual bool onClose() = 0;
};
