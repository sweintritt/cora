#pragma once

#include "commands/command.hpp"

class ListCommand : public Command {
    public:
        ListCommand(const std::shared_ptr<StationsDao> stationsDao, 
                    const std::shared_ptr<SettingsDao> settingsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);
        void execute(const std::vector<std::string>& args) override;
};