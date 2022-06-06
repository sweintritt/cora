#pragma once

#include "commands/command_interpreter.hpp"

#include <string>
#include <vector>

class Cora {
    public:
        Cora();
        void run(const std::vector<std::string>& args);

    private:
        std::shared_ptr<CommandInterpreter> m_commandInterpreter;
        void runCommand(const std::vector<std::string>& args);
        void configureLogger() const;
};