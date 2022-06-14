#pragma once

#include "commands/command.hpp"

class InfoCommand : public Command {
    public:
        InfoCommand(const std::shared_ptr<StationsDao> stationsDao, 
                    const std::shared_ptr<SettingsDao> settingsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);
        void execute(const std::vector<std::string>& args) override;

    private:
        std::string formatDescription(const std::string& description) const;
};