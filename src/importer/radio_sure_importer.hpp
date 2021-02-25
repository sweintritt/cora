#pragma once

#include "importer/importer.hpp"

class RadioSureImporter : public Importer {
    public:
        RadioSureImporter(const std::shared_ptr<StationsDao> stationsDao);

        virtual ~RadioSureImporter();

        virtual void import(const std::string& url) override;
};