#pragma once

#include "db/settings.hpp"
#include "db/dao.hpp"

#include <sqlite3.h>

class SettingsDao : public Dao {
    public:
        SettingsDao() = default;
        virtual ~SettingsDao() = default;
        virtual void save(const std::string& key, const std::string& value);
        virtual std::string get(const std::string& key);
    
    protected:
        void onOpen() override;
        bool onClose() override;

    private:
        sqlite3_stmt* saveStmnt;
        sqlite3_stmt* getStmnt;
};
