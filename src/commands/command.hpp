#pragma once

#include <vector>
#include <string>
#include <memory>

#include "stations_dao.hpp"
#include "media_player.hpp"
#include "cli/cli.hpp"

class Command {
    public:
        Command(const std::string& name, const std::string& description);

        virtual ~Command();

        virtual void execute(const std::vector<std::string>& args) = 0;

        virtual const std::string& getName() const;

        virtual std::string getUsage() const;

    protected:
        const std::string m_name;

        Cli m_cli;

        virtual const std::string getDefaultFile() const;

        virtual std::shared_ptr<MediaPlayer> createPlayer() const;

        virtual std::shared_ptr<StationsDao> createStationsDao() const;
};

