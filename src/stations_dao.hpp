#pragma once

#include <string>

#include "station.hpp"

class StationsDao {
    public:
        virtual ~StationsDao() {};

        virtual void open(const std::string& url) = 0;

        virtual void create(const std::string& url) = 0;

        virtual void close() = 0;

        virtual void save(const Station& station) = 0;

        virtual Station get(const long id) = 0;

        virtual Station getRandom() = 0;

        virtual int getVersion() = 0;

        virtual void create() = 0;

        virtual void upgrade(const int oldVersion, const int newVersion) = 0;

        virtual std::vector<Station> find(const std::string& name, const std::string& genre, const std::string& language, const std::string& country) = 0;

        virtual std::vector<std::string> getGenres() = 0;

        virtual std::vector<std::string> getCountries() = 0;

        virtual std::vector<std::string> getLanguages() = 0;

};
