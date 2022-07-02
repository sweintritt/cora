#include "testutils/in_memory_stations_dao.hpp"

#include <stdexcept>
#include <functional>
#include <sstream>

void InMemoryStationsDao::onOpen() {
    m_id = 0;
}

bool InMemoryStationsDao::onClose() {
    m_stations.clear();
    return true;
}

void InMemoryStationsDao::save(Station& station) {
    station.setId(m_id);
    ++m_id;
    m_stations.emplace(station.getId(), std::make_shared<Station>(station));
}

std::vector<long> InMemoryStationsDao::find(const std::string& name, const std::string& genre, const std::string& country) {
    // TODO Implement
    throw std::runtime_error("not implemented");
}

std::shared_ptr<Station> InMemoryStationsDao::findById(const long id) {
    return m_stations[id];
}

void InMemoryStationsDao::deleteAllAddedBy(const std::string& addedBy) {
    std::map<long, std::shared_ptr<Station>> stations;
    for (auto& station : m_stations) {
        if (station.second->getAddedBy().compare(addedBy) != 0) {
            stations.emplace(station.first, station.second);
        }
    }
    m_stations = stations;
}

std::shared_ptr<Station> InMemoryStationsDao::getRandom() {
    if (m_stations.empty()) {
        return nullptr;
    } else {
        return m_stations[0];
    }
}

std::vector<long> InMemoryStationsDao::getAllIds() {
    std::vector<long> keys;

    for(const auto& entry : m_stations) {
        keys.push_back(entry.first);
    }

    return keys;
}
