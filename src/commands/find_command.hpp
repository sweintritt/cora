#pragma once

#include "commands/command.hpp"

class FindCommand : public Command {
    public:
        FindCommand(const std::shared_ptr<StationsDao> stationsDao, 
                    const std::shared_ptr<SettingsDao> settingsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);
        void execute(const std::vector<std::string>& args) override;
};