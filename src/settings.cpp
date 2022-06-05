#include "settings.hpp"

const std::string Settings::LAST_PLAYED = std::string{"last.played"};

std::string Settings::get(const std::string& key) {
    return settings[key];
}

int Settings::getAsInt(const std::string& key) {
    return stoi(settings[key]);
}