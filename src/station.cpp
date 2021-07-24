#include "station.hpp"

Station::Station()
    : m_id()
    , m_addedBy()
    , m_name()
    , m_genre()
    , m_country()
    , m_language()
    , m_description()
    , m_urls() { }

Station::~Station() { }

void Station::setId(const int64_t id) {
    m_id = id;
}

int64_t Station::getId() const {
    return m_id;
}

void Station::setAddedBy(const std::string& addedBy) {
    m_addedBy = addedBy;
}

const std::string& Station::getAddedBy() const {
    return m_addedBy;
}

void Station::setName(const std::string& name) {
    m_name = name;
}

const std::string& Station::getName() const {
    return m_name;
}

void Station::setGenre(const std::string& genre) {
    m_genre = genre;
}

const std::string& Station::getGenre() const {
    return m_genre;
}

void Station::setCountry(const std::string& country) {
    m_country = country;
}

const std::string& Station::getCountry() const {
    return m_country;
}

void Station::setLanguage(const std::string& language) {
    m_language = language;
}

const std::string& Station::getLanguage() const {
    return m_language;
}

void Station::setDescription(const std::string& description) {
    m_description = description;
}

const std::string& Station::getDescription() const {
    return m_description;
}


void Station::addUrl(const std::string& url) {
    m_urls.push_back(url);
}

const std::vector<std::string>& Station::getUrls() const {
    return m_urls;
}
