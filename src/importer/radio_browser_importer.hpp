#pragma once

#include "importer/importer.hpp"

class RadioBrowserImporter : public Importer {
    public:
        RadioBrowserImporter();

        virtual ~RadioBrowserImporter();

        virtual void import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) override;

    private:
        virtual std::string getStationsJson(const std::string& url);
        virtual std::vector<Station> parseJson(const std::string& json);
};