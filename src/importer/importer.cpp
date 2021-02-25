#include "importer/importer.hpp"

Importer::Importer(const std::string& name, const std::shared_ptr<StationsDao> stationsDao)
    : m_name(name)
    , m_stationsDao(stationsDao) {
}

Importer::~Importer() {

}

const std::string& Importer::getName() {
    return m_name;
}