#include "station.hpp"

Station::Station()
    : idHash()
    , author(IMPORT)
    , name()
    , genre()
    , country()
    , language()
    , description()
    , urls() { }

Station::~Station() {

}

void Station::setIdHash(const long idHash) {
    this->idHash = idHash;
}

long Station::getIdHash() {
    return idHash;
}

void Station::setAuthor(const Author author) {
    this->author = author;
}

Author Station::getAuthor() {
    return author;
}

void Station::setName(const std::string& name) {
    this->name = name;
}

const std::string& Station::getName() {
    return name;
}

void Station::setGenre(const std::string& genre) {
    this->genre = genre;
}

const std::string& Station::getGenre() {
    return genre;
}

void Station::setCountry(const std::string& country) {
    this->country = country;
}

const std::string& Station::getCountry() {
    return country;
}

void Station::setLanguage(const std::string& language) {
    this->language = language;
}

const std::string& Station::getLanguage() {
    return language;
}

void Station::setDescription(const std::string& description) {
    this->description = description;
}

const std::string& Station::getDescription() {
    return description;
}


void Station::addUrl(const std::string& url) {
    this->urls.push_back(url);
}

const std::vector<std::string>& Station::getUrls() {
    return urls;
}
