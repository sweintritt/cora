#include "settings.hpp"

const std::string Settings::LAST_PLAYED = std::string{"last.played"};
const std::string Settings::CORA_VERSION = std::string{"cora.version"};
const std::string Settings::LAST_UPDATE = std::string{"lars.update"};

void Settings::set(const std::string& name, const std::string& value) {
    settings[name] = value;
}

std::string Settings::get(const std::string& name) {
    return settings[name];
}

int Settings::getAsInt(const std::string& name) {
    return stoi(settings[name]);
}

const std::map<std::string, std::string>& Settings::all() {
    return settings;
}