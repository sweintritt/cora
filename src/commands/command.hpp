#pragma once

#include <vector>
#include <string>
#include <memory>

#include "db/stations_dao.hpp"
#include "db/settings_dao.hpp"
#include "cli/cli.hpp"
#include "player/media_player.hpp"

class Command {
    public:
        Command(const std::string& name, 
                const std::string& description, 
                const std::shared_ptr<StationsDao> stationsDao, 
                const std::shared_ptr<SettingsDao> settingsDao,
                const std::shared_ptr<MediaPlayer> mediaPlayer);
        virtual ~Command() = default;
        virtual void execute(const std::vector<std::string>& args) = 0;
        virtual const std::string& getName() const;
        virtual std::string getDescription() const;
        virtual std::string getUsage() const;

    protected:
        const std::string m_name;
        Cli m_cli;
        std::shared_ptr<StationsDao> m_stationsDao;
        std::shared_ptr<SettingsDao> m_settingsDao;
        std::shared_ptr<MediaPlayer> m_mediaPlayer;

        std::string getDefaultFile() const;
};

