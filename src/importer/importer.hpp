#pragma once

#include <string>

#include "stations_dao.hpp"

class Importer {
    public:
        Importer(const std::string& name);

        virtual ~Importer();

        virtual void import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) = 0;

        const std::string& getName() const;

    protected:
        std::string m_name;
};