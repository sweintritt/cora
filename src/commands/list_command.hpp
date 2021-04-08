#pragma once

#include "commands/command.hpp"

class ListCommand : public Command {
    public:
        ListCommand();

        virtual ~ListCommand();

        virtual void execute(const std::vector<std::string>& args) override;
};