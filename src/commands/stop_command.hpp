#pragma once

#include "commands/command.hpp"

class StopCommand : public Command {
    public:
        StopCommand(const std::shared_ptr<StationsDao> stationsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);

        virtual ~StopCommand();

        virtual void execute(const std::vector<std::string>& args) override;
};