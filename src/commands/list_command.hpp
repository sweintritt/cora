#pragma once

#include "commands/command.hpp"

class ListCommand : public Command {
    public:
        ListCommand(const std::shared_ptr<Settings> settings);
        virtual void execute(const std::vector<std::string>& args) override;
};