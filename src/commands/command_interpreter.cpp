#include "commands/command_interpreter.hpp"

#include <stdexcept>

CommandInterpreter::CommandInterpreter()
    : m_commands() {
}

CommandInterpreter::~CommandInterpreter() { }

void CommandInterpreter::add(std::unique_ptr<Command> command) {
    // TODO Verify command is not already there
    m_commands.insert(std::make_pair(command->getName(), std::move(command)));
}

void CommandInterpreter::execute(const std::vector<std::string>& args) {
    auto it = m_commands.find(args[0]);

    if (it != m_commands.end()) {
        it->second->execute(args);
    } else {
        throw std::invalid_argument("unknown command: " + args[0]);
    }
}
