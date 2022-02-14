#pragma once

#include "stations_dao.hpp"

#include <sqlite3.h>

class SqliteStationsDao : public StationsDao {
    public:
        SqliteStationsDao();

        virtual ~SqliteStationsDao() override;

        virtual void open(const std::string& url) override;

        virtual void close() override;

        virtual void save(Station& station) override;

        virtual std::vector<long> find(const std::string& value) override;

        virtual std::shared_ptr<Station> findById(const long id) override;

        virtual void deleteAllAddedBy(const std::string& addedBy) override;

        virtual std::shared_ptr<Station> getRandom() override;

        virtual int getVersion() override;

        virtual void create() override;

        virtual void upgrade(const int oldVersion, const int newVersion) override;

        virtual std::vector<long> getAllIds() override;

        virtual void beginTransaction() override;

        virtual void commit() override;

        virtual void rollback() override;

    private:
        int version;

        std::string file;

        sqlite3* db;

        sqlite3_stmt* insertStationStmnt;

        sqlite3_stmt* findStationByIdStmnt;

        sqlite3_stmt* deleteByAddedByStmnt;

        sqlite3_stmt* getAllIdsStmnt;

        sqlite3_stmt* getRandomStationStmnt;

        std::string getError();

        void prepare(sqlite3_stmt** prepared, const std::string& stmnt);

        std::string serializeUrls(const std::vector<std::string>& urls);

        std::vector<std::string> deserializeUrls(const std::string& urls);

        Station getStation(sqlite3_stmt* stmnt);
};
