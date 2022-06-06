#pragma once

#include "commands/command.hpp"
#include "commands/command_interpreter.hpp"

#include <memory>
#include <string>

class HelpCommand : public Command {
    public:
        explicit HelpCommand(const std::shared_ptr<CommandInterpreter> commandInterpreter);
        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::shared_ptr<CommandInterpreter> m_commandInterpreter;
};