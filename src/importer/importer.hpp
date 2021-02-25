#pragma once

#include <string>

#include "stations_dao.hpp"

class Importer {
    public:
        Importer(const std::string& name, const std::shared_ptr<StationsDao> stationsDao);

        virtual ~Importer();

        virtual void import(const std::string& url) = 0;

        const std::string& getName();

    protected:
        std::shared_ptr<StationsDao> m_stationsDao;

        std::string m_name;
};