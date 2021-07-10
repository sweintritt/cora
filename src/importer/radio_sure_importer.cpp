#include "importer/radio_sure_importer.hpp"
#include "utils.hpp"

#include <plog/Log.h>

#include <iostream>
#include <fstream>
#include <vector>

RadioSureImporter::RadioSureImporter()
    : Importer("radio-sure") {}

RadioSureImporter::~RadioSureImporter() {}

void RadioSureImporter::import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) {
    std::string line;
    std::ifstream file(url);

    if (file.is_open()) {
        long count = 0;
        std::time_t start = std::time(nullptr);
        // TODO transaction
        while (getline(file, line)) {
            const std::vector<std::string> values = split(line, '\t');

            Station station;
            station.setName(values[0]);
            station.setDescription(values[1]);
            station.setGenre(values[2]);
            station.setCountry(values[3]);
            station.setLanguage(values[4]);
            station.setAddedBy(getName());

            for (unsigned int i = 5; i < values.size(); ++i) {
              if (values[i].compare("-")) {
                station.addUrl(values[i]);
              }
            }

            stationsDao->save(station);
            ++count;
        }

        LOG(plog::info) << count << " stations imported in " << std::difftime(std::time(nullptr), start) << " s";
        file.close();
    } else {
        throw std::runtime_error("Unable to open file " + url);
    }
}