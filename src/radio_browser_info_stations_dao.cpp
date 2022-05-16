#include "radio_browser_info_stations_dao.hpp"

#include <string>
#include <curl/curl.h>
#include <plog/Log.h>

// API doc: https://de1.api.radio-browser.info/
// all stations: https://de1.api.radio-browser.info/json/stations

/**
 * 
    http://de1.api.radio-browser.info/{format}/stations/byuuid/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/byname/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bynameexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bycodec/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bycodecexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bycountry/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bycountryexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bycountrycodeexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bystate/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bystateexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bylanguage/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bylanguageexact/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bytag/{searchterm}
    http://de1.api.radio-browser.info/{format}/stations/bytagexact/{searchterm}
 */

// http://de1.api.radio-browser.info/json/stations/search
// Parameters:
// country 		STRING
// name 		STRING
// language     STRING
// limit 	100000 	0,1,2,...

RadioBrowserInfoStationsDao::RadioBrowserInfoStationsDao() {

}

RadioBrowserInfoStationsDao::~RadioBrowserInfoStationsDao() {

}

void RadioBrowserInfoStationsDao::open(const std::string& url) {

}

void RadioBrowserInfoStationsDao::close() {

}

void RadioBrowserInfoStationsDao::save(Station& station) {

}

static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
   LOG(plog::debug) << "adding " << size;
   ((std::string*)userp)->append((char*)contents, size * nmemb);
   return size * nmemb;
}

std::vector<long> RadioBrowserInfoStationsDao::find(const std::string& name, const std::string& genre, const std::string& country) {
   // TODO Implement
   // http://de1.api.radio-browser.info/json/stations/search
   // Parameters:
   // country 		STRING
   // name 		STRING
   // language     STRING
   CURL *curl;
   CURLcode res;
   std::string readBuffer;

   curl_global_init(CURL_GLOBAL_DEFAULT);
   curl = curl_easy_init();
   if(curl) {
      // https://curl.se/libcurl/c/curl_easy_setopt.html
      const std::string url = "http://de1.api.radio-browser.info/json/stations/search?name=" + name;
      LOG(plog::debug) << "urls:" << url;
      // curl_easy_setopt(curl, CURLOPT_URL, "http://de1.api.radio-browser.info/json/stations/search?name=" + name);
      // curl_easy_setopt(curl, CURLOPT_URL, "http://de1.api.radio-browser.info/json/stations/search?name=foo");
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
      // TODO for foo i get some results in the browser but not here
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "com.github/sweintritt/cora");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      LOG(plog::debug) << "perform";
      res = curl_easy_perform(curl);
      if(res != CURLE_OK) {
         LOG(plog::error) << "curl_easy_perform() failed: " << curl_easy_strerror(res);
      }

      LOG(plog::debug) << "cleanup";
      curl_easy_cleanup(curl);
      LOG(plog::debug) << "got (rc:" << res << ", name:" << name << "): " << readBuffer;
   }

   return std::vector<long>{};
}

std::shared_ptr<Station> RadioBrowserInfoStationsDao::findById(const long id) {
   // TODO Implement
   return std::shared_ptr<Station>{};
}

void RadioBrowserInfoStationsDao::deleteAllAddedBy(const std::string& addedBy) {
   // TODO Implement
}

std::shared_ptr<Station> RadioBrowserInfoStationsDao::getRandom() {
   // TODO Implement
   return std::shared_ptr<Station>{};
}

int RadioBrowserInfoStationsDao::getVersion() {
   // TODO Implement
   return 0;
}

void RadioBrowserInfoStationsDao::create() {
   // TODO Implement
}

void RadioBrowserInfoStationsDao::upgrade(const int oldVersion, const int newVersion) {
   // TODO Implement
}

std::vector<long> RadioBrowserInfoStationsDao::getAllIds() {
   // TODO Implement
}

void RadioBrowserInfoStationsDao::beginTransaction() {
   // TODO Implement
}

void RadioBrowserInfoStationsDao::commit() {
   // TODO Implement
}

void RadioBrowserInfoStationsDao::rollback() {
   // TODO Implement
}
