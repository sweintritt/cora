#include "importer/radio_sure_importer.hpp"
#include "utils.hpp"

#include <plog/Log.h>

#include <iostream>
#include <fstream>
#include <vector>

RadioSureImporter::RadioSureImporter(const std::shared_ptr<StationsDao> stationsDao)
    : Importer("radio-sure", stationsDao) {}

RadioSureImporter::~RadioSureImporter() {}

void RadioSureImporter::import(const std::string& url) {
    std::string line;
    std::ifstream file(url);

    if (file.is_open()) {
        while (getline(file, line)) {
            const std::vector<std::string> values = split(line, '\t');

            Station station;
            station.setName(values[0]);
            station.setDescription(values[1]);
            station.setGenre(values[2]);
            station.setCountry(values[3]);
            station.setLanguage(values[4]);
            station.setAuthor(Author::IMPORT);
            // TODO urls are not stored. compare might be wrong
            for (unsigned int i = 5; i < values.size(); ++i) {
              if (values[i].compare("-")) {
                station.addUrl(values[i]);
              }
            }

            m_stationsDao->save(station);
        }
        file.close();
    } else {
        throw std::runtime_error("Unable to open file " + url);
    }
}