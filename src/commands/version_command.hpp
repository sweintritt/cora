#pragma once

#include "commands/command.hpp"

class VersionCommand : public Command {
    public:
        VersionCommand();

        virtual ~VersionCommand();

        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::string findId(const std::vector<std::string>& args) const;
};