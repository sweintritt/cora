#pragma once

#include "commands/command.hpp"

class InfoCommand : public Command {
    public:
        InfoCommand(const std::shared_ptr<Settings> settings);
        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::string formatDescription(const std::string& description) const;
};