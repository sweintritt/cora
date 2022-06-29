#include "testutils/in_memory_settings_dao.hpp"

void InMemorySettingsDao::onOpen() {
    // Do nothing
}

bool InMemorySettingsDao::onClose() {
    // Do nothing
    return true;
}

void InMemorySettingsDao::save(const std::string& key, const std::string& value) {
    m_settings.emplace(key, value);
}

std::string InMemorySettingsDao::get(const std::string& key) {
    auto it = m_settings.find(key);
    
    if (it != m_settings.end()) {
        return it->second;
    }

    return "";
}

