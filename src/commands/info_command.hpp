#pragma once

#include "commands/command.hpp"

class InfoCommand : public Command {
    public:
        InfoCommand();

        virtual ~InfoCommand();

        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::string findId(const std::vector<std::string>& args) const;
};