#pragma once

#include "commands/command_interpreter.hpp"

#include <string>
#include <vector>

class Cora {
    public:
        Cora();
        void run(const std::vector<std::string>& args);

    private:
        std::shared_ptr<CommandInterpreter> m_commandInterpreter;
        std::shared_ptr<StationsDao> m_stationsDao;
        std::shared_ptr<SettingsDao> m_settingsDao;
        std::shared_ptr<MediaPlayer> m_mediaPlayer;
        
        void runCommand(const std::vector<std::string>& args);
        void configureLogger() const;
};