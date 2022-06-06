#pragma once

#include "commands/command.hpp"

class FindCommand : public Command {
    public:
        FindCommand();
        virtual void execute(const std::vector<std::string>& args) override;
};