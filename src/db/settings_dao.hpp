#pragma once

#include "db/settings.hpp"
#include "db/dao.hpp"

#include <sqlite3.h>

class SettingsDao : public Dao {
    public:
        virtual void save(const std::string& key, const std::string& value);
        virtual std::string get(const std::string& key);
    
    protected:
        virtual void onOpen() override;
        virtual bool onClose() override;

    private:
        sqlite3_stmt* saveStmnt;
        sqlite3_stmt* getStmnt;
};
