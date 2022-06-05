#pragma once

#include <string>

#include "stations_dao.hpp"

class Importer {
    public:
        explicit Importer(const std::string& name);

        virtual ~Importer() = default;

        virtual void import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) = 0;

        const std::string& getName() const;

    private:
        std::string name;
};