#include "commands/command_interpreter.hpp"

#include <plog/Log.h>
#include <stdexcept>

CommandInterpreter::CommandInterpreter()
    : m_commands() {
}

CommandInterpreter::~CommandInterpreter() { }

void CommandInterpreter::add(std::unique_ptr<Command> cmd) {
    if (hasCommand(cmd->getName())) {
        throw std::runtime_error("duplicate command " + cmd->getName());
    }
    m_commands.insert(std::make_pair(cmd->getName(), std::move(cmd)));
}

void CommandInterpreter::execute(const std::vector<std::string>& args) {
    auto it = m_commands.find(args[1]);

    if (it != m_commands.end()) {
        it->second->execute(args);
    } else {
        throw std::invalid_argument("Unknown command: " + args[1]);
    }
}

bool CommandInterpreter::hasCommand(const std::string& cmd) {
    return m_commands.find(cmd) != m_commands.end();
}

void CommandInterpreter::showCommands() const {
    for (auto const& command : m_commands) {
        std::string name{command.second->getName()};
        // TODO Find max name length
        name.resize(10, ' ');
        LOG(plog::info) << "   " << name << command.second->getDescription();
    }
}