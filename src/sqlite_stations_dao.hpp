#pragma once

#include "stations_dao.hpp"

#include <sqlite3.h>

class SqliteStationsDao : public StationsDao {
    public:
        virtual ~SqliteStationsDao() override;

        virtual void open(const std::string& url) override;

        virtual void create(const std::string& url) override;

        virtual void close() override;

        virtual void save(const Station& station) override;

        virtual Station get(const long id) override;

        virtual Station getRandom() override;

        virtual int getVersion() override;

        virtual void create() override;

        virtual void upgrade(const int oldVersion, const int newVersion) override;

        virtual std::vector<Station> find(const std::string& name, const std::string& genre, const std::string& language, const std::string& country) override;

        virtual std::vector<std::string> getGenres() override;

        virtual std::vector<std::string> getCountries() override;

        virtual std::vector<std::string> getLanguages() override;

    private:
        int version;

        sqlite3* db;

        std::string getError();
};
