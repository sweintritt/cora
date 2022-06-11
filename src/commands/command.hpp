#pragma once

#include <vector>
#include <string>
#include <memory>

#include "db/stations_dao.hpp"
#include "db/settings.hpp"
#include "cli/cli.hpp"
#include "player/media_player.hpp"

class Command {
    public:
        Command(const std::string& name, const std::string& description, std::shared_ptr<Settings> settings);
        virtual ~Command() = default;
        virtual void execute(const std::vector<std::string>& args) = 0;
        virtual const std::string& getName() const;
        virtual std::string getDescription() const;
        virtual std::string getUsage() const;

    protected:
        const std::string m_name;
        std::shared_ptr<Settings> m_settings;
        Cli m_cli;

        std::string getDefaultFile() const;
        virtual std::shared_ptr<MediaPlayer> createPlayer() const;
        virtual std::shared_ptr<StationsDao> createStationsDao() const;
        virtual std::shared_ptr<StationsDao> createSettingsDao() const;
};

