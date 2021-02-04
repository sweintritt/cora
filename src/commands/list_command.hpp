#pragma once

#include "commands/command.hpp"

class ListCommand : public Command {
    public:
        ListCommand(const std::shared_ptr<StationsDao> stationsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);

        virtual ~ListCommand();

        virtual void execute(const std::vector<std::string>& args) override;
};