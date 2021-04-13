#include "commands/help_command.hpp"

#include <plog/Log.h>

HelpCommand::HelpCommand(const std::shared_ptr<CommandInterpreter> commandInterpreter) : Command("help", "Show help page"), m_commandInterpreter(commandInterpreter) {
}

HelpCommand::~HelpCommand() { }

void HelpCommand::execute(const std::vector<std::string>& args) {
    LOG(plog::info) << "cora - listen to internet radio stations";
    m_commandInterpreter->showCommands();
}