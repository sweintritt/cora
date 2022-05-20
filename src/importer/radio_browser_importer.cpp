#include "importer/radio_browser_importer.hpp"
#include "utils.hpp"

#include <curl/curl.h>
#include <plog/Log.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <vector>

RadioBrowserImporter::RadioBrowserImporter()
    : Importer("radio-browser") {}

RadioBrowserImporter::~RadioBrowserImporter() {}

void RadioBrowserImporter::import(const std::string& url, const std::shared_ptr<StationsDao> stationsDao) {
    std::string line;
    std::ifstream file(url);

    if (file.is_open()) {
        const std::time_t start = std::time(nullptr);
        stationsDao->beginTransaction();
        // All entries added by the last RadioBrowser import will be removed
        stationsDao->deleteAllAddedBy(getName());

        try {
            std::string json;

            if (url.empty()) {
                json = getStationsJson("https://de1.api.radio-browser.info/json/stations");
            } else {
                json = getStationsJson(url);
            }

            std::vector<Station> stations = parseJson(json);
            for (auto& station : stations) {
                stationsDao->save(station);
            }

            LOG(plog::info) << stations.size() << " stations imported in " << std::difftime(std::time(nullptr), start) << " s";
            stationsDao->commit();
        } catch (const std::exception& e) {
            stationsDao->rollback();
            throw;
        } catch (const std::string& e) {
            stationsDao->rollback();
            throw;
        } catch (...) {
            stationsDao->rollback();
            LOG(plog::error) << "Unknown error";
        }
    } else {
        throw std::runtime_error("Unable to open file " + url);
    }
}

static size_t writeCallback(void* content, size_t size, size_t nmemb, void* data) {
    const size_t length = size*nmemb;
    ((std::string*) data)->append((char*) content, length);
    return length;
}

std::string RadioBrowserImporter::getStationsJson(const std::string& url) {
   CURL *curl;
   CURLcode res;
   std::string json;

   curl = curl_easy_init();
   if(curl) {
      // https://curl.se/libcurl/c/curl_easy_setopt.html
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "com.github/sweintritt/cora");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      LOG(plog::debug) << "perform";
      res = curl_easy_perform(curl);

      if(res != CURLE_OK) {
         LOG(plog::error) << "error while loading stations: " << curl_easy_strerror(res);
            return "";
        }

        curl_easy_cleanup(curl);
        return json;  
    } else {
        LOG(plog::error) << "error while loading stations";
        return "";
    }
}

std::vector<Station> RadioBrowserImporter::parseJson(const std::string& json) {
    nlohmann::json array = json;
    std::vector<Station> stations;

    for (auto& entry : array) {
        Station station;
        station.setName(entry["name"]);
        station.setDescription(entry["name"]);
        station.setGenre(entry["tags"]);
        station.setCountry(entry["country"]);
        station.setLanguage(entry["language"]);
        station.setAddedBy(getName());
        station.addUrl(entry["url_resolved"]);
        station.addUrl(entry["url"]);

        stations.push_back(station);
    }

    return stations;
}