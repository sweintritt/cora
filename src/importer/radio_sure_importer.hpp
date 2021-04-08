#pragma once

#include "importer/importer.hpp"

class RadioSureImporter : public Importer {
    public:
        RadioSureImporter();

        virtual ~RadioSureImporter();

        virtual void import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) override;
};