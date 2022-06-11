#pragma once

#include "commands/command.hpp"

class FindCommand : public Command {
    public:
        FindCommand(const std::shared_ptr<Settings> settings);
        virtual void execute(const std::vector<std::string>& args) override;
};