#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "commands/command.hpp"

class CommandInterpreter {
    public:
        CommandInterpreter();

        virtual ~CommandInterpreter();

        void add(std::unique_ptr<Command> cmd);

        void execute(const std::vector<std::string>& args);

        bool hasCommand(const std::string& cmd);
    private:
        std::map<std::string, std::unique_ptr<Command>> m_commands;
};
