#include "station.hpp"

Station::Station() : urls() {

}


long Station::getId() const {
    return id;
}


void Station::setId(long id) {
    this->id = id;
}


const std::string& Station::getName() const {
    return name;
}

void Station::setName(const std::string& name) {
    this->name = name;
}

const std::string& Station::getDescription() const {
    return description;
}

void Station::setDescription(const std::string& description) {
    this->description = description;
}

const std::string& Station::getGenre() const {
    return genre;
}

void Station::setGenre(const std::string& genre) {
    this->genre = genre;
}

const std::string& Station::getCountry() const {
    return country;
}

void Station::setCountry(const std::string& country) {
    this->country = country;
}

const std::string& Station::getLanguage() const {
    return language;
}

void Station::setLanguage(const std::string& language) {
    this->language = language;
}

const std::vector<std::string>& Station::getUrls() const {
    return urls;
}

std::vector<std::string> Station::getMutableUrls() {
    return urls;
}
