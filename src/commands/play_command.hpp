#pragma once

#include "commands/command.hpp"

class PlayCommand : public Command {
    public:
        PlayCommand(const std::shared_ptr<StationsDao> stationsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);

        virtual ~PlayCommand();

        virtual void execute(const std::vector<std::string>& args) override;
};