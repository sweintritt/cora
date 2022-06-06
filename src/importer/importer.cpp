#include "importer/importer.hpp"

Importer::Importer(const std::string& importerName) : name(importerName) {
}

const std::string& Importer::getName() const {
    return name;
}