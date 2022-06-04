#pragma once

#include "stations_dao.hpp"
#include "sqlite3_dao.hpp"

#include <sqlite3.h>

class SqliteStationsDao : public StationsDao, Sqlite3Dao {
    public:
        SqliteStationsDao();
        virtual ~SqliteStationsDao() = default;
        //virtual void open(const std::string& url) override;
        //virtual void close() override;
        virtual void save(Station& station) override;
        virtual std::vector<long> find(const std::string& name, const std::string& genre, const std::string& country) override;
        virtual std::shared_ptr<Station> findById(const long id) override;
        virtual void deleteAllAddedBy(const std::string& addedBy) override;
        virtual std::shared_ptr<Station> getRandom() override;
        virtual std::vector<long> getAllIds() override;
    
    protected:
        virtual void onOpen() override;
        virtual bool onClose() override;

    private:
        sqlite3_stmt* insertStationStmnt;
        sqlite3_stmt* findStationByIdStmnt;
        sqlite3_stmt* findStationStmnt;
        sqlite3_stmt* deleteByAddedByStmnt;
        sqlite3_stmt* getAllIdsStmnt;
        sqlite3_stmt* getRandomStationStmnt;

        std::string serializeUrls(const std::vector<std::string>& urls);
        std::vector<std::string> deserializeUrls(const std::string& urls);
        Station getStation(sqlite3_stmt* stmnt);
};
