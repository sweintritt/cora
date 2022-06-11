#pragma once

#include "commands/command.hpp"

class VersionCommand : public Command {
    public:
        VersionCommand(const std::shared_ptr<Settings> settings);
        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::string findId(const std::vector<std::string>& args) const;
};