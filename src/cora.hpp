#pragma once

#include "cli/cli.hpp"
#include "commands/command_interpreter.hpp"
#include "stations_dao.hpp"
#include "media_player.hpp"

#include <string>
#include <memory>

class Cora {
    public:
        Cora();
        ~Cora();
        void run(int argc, char* argv[]);
    private:
        Cli m_cli;
        CommandInterpreter m_commandInterpreter;
        std::shared_ptr<StationsDao> m_stationsDao;
        std::shared_ptr<MediaPlayer> m_mediaPlayer;

        std::string getDefaultFile();
        void configureLogger(const bool debug);
        void addStations();
        void runInteractive();
        void runCommand(const std::vector<std::string>& args);

};