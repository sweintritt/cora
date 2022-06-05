#include "importer/importer.hpp"

Importer::Importer(const std::string& name) : name(name) {
}

const std::string& Importer::getName() const {
    return name;
}