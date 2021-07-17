#pragma once

#include "commands/command.hpp"

class PlayCommand : public Command {
    public:
        PlayCommand();

        virtual ~PlayCommand();

        virtual void execute(const std::vector<std::string>& args) override;
};