#pragma once

#include "commands/command_interpreter.hpp"

#include <string>
#include <vector>

class Cora {
    public:
        Cora();

        ~Cora();

        void run(int argc, char* argv[]);

    private:
        std::shared_ptr<CommandInterpreter> m_commandInterpreter;

        void addStations();

        void runCommand(const std::vector<std::string>& args);

        void configureLogger() const;
};