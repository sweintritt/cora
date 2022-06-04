#pragma once

#include <map>
#include <string>

class Settings {
    public:
        static const std::string LAST_PLAYED;

        Settings() = default;
        ~Settings() = default;

        std::string get(const std::string& key);
        int getAsInt(const std::string& key);
    private:
        std::map<std::string, std::string> settings;
}