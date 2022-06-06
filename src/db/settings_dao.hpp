#pragma once

#include "db/settings.hpp"
#include "db/dao.hpp"

#include <sqlite3.h>

class SettingsDao : public Dao {
    public:
        SettingsDao();
        virtual ~SettingsDao() = default;
        virtual void save(Settings& settings);
        virtual Settings load();
    
    protected:
        virtual void onOpen() override;
        virtual bool onClose() override;

    private:
        sqlite3_stmt* insertSettingStmnt;
        sqlite3_stmt* loadSettingsStmnt;
};
