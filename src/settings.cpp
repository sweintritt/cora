#include "settings.hpp"

static const std::string LAST_PLAYED = "last.played";

std::string Settings::get(const std::string& key) {
    return settings[key];
}

int Settings::getAsInt(const std::string& key) {
    return stoi(settings[key]);
}