#pragma once

#include <map>
#include <string>

class Settings {
    public:
        static const std::string LAST_PLAYED;
        static const std::string CORA_VERSION;
        static const std::string LAST_UPDATE;

        Settings() = default;
        ~Settings() = default;

        void set(const std::string& name, const std::string& value);
        std::string get(const std::string& name);
        int getAsInt(const std::string& name);
        const std::map<std::string, std::string>& all() const;
    private:
        std::map<std::string, std::string> settings;
};