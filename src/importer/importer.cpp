#include "importer/importer.hpp"

Importer::Importer(const std::string& name) : m_name(name) {
}

Importer::~Importer() {

}

const std::string& Importer::getName() {
    return m_name;
}