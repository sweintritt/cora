#include "station.hpp"

Station::Station()
    : id()
    , idHash()
    , author(IMPORT)
    , name()
    , genre()
    , country()
    , language()
    , description()
    , urls() { }

Station::~Station() {

}

void Station::setId(const long id) {
    this->id = id;
}

long Station::getId() const {
    return id;
}

void Station::setIdHash(const long idHash) {
    this->idHash = idHash;
}

long Station::getIdHash() const {
    return idHash;
}

void Station::setAuthor(const Author author) {
    this->author = author;
}

Author Station::getAuthor() const {
    return author;
}

void Station::setName(const std::string& name) {
    this->name = name;
}

const std::string& Station::getName() const {
    return name;
}

void Station::setGenre(const std::string& genre) {
    this->genre = genre;
}

const std::string& Station::getGenre() const {
    return genre;
}

void Station::setCountry(const std::string& country) {
    this->country = country;
}

const std::string& Station::getCountry() const {
    return country;
}

void Station::setLanguage(const std::string& language) {
    this->language = language;
}

const std::string& Station::getLanguage() const {
    return language;
}

void Station::setDescription(const std::string& description) {
    this->description = description;
}

const std::string& Station::getDescription() const {
    return description;
}


void Station::addUrl(const std::string& url) {
    this->urls.push_back(url);
}

const std::vector<std::string>& Station::getUrls() const {
    return urls;
}
