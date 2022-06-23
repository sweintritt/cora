#pragma once

#include "db/settings_dao.hpp"

#include <map>

class InMemorySettingsDao : public SettingsDao {
    public:
        InMemorySettingsDao() = default;
        virtual ~InMemorySettingsDao() = default;
        void save(const std::string& key, const std::string& value) override;
        std::string get(const std::string& key) override;
    
    protected:
        void onOpen() override;
        bool onClose() override;

    private:
        std::map<std::string, std::string> m_settings;
};
