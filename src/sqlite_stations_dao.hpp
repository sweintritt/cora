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

        virtual std::shared_ptr<Station> findById(const long id) override;

        virtual std::shared_ptr<Station> getRandom() override;

        virtual int getVersion() override;

        virtual void create() override;

        virtual void upgrade(const int oldVersion, const int newVersion) override;

        virtual std::vector<Station> find(const std::string& name, const std::string& genre, const std::string& language, const std::string& country, const int limit) override;

        virtual std::vector<std::string> getGenres() override;

        virtual std::vector<std::string> getCountries() override;

        virtual std::vector<std::string> getLanguages() override;

        virtual std::vector<long> getAllIds() override;

    private:
        int version;

        std::string file;

        sqlite3* db;

        sqlite3_stmt* insertStationStmnt;

        sqlite3_stmt* findStationByIdStmnt;

        sqlite3_stmt* findStationsStmnt;

        sqlite3_stmt* getAllIdsStmnt;

        std::string getError();

        void prepare(sqlite3_stmt** prepared, const std::string stmnt);

        std::string serializeUrls(const std::vector<std::string>& urls);

        std::vector<std::string> deserializeUrls(const std::string& urls);

        Station getStation(sqlite3_stmt* stmnt);
};
