#include "commands/help_command.hpp"

#include <plog/Log.h>

HelpCommand::HelpCommand(const std::shared_ptr<CommandInterpreter> commandInterpreter, 
                         const std::shared_ptr<StationsDao> stationsDao, 
                         const std::shared_ptr<SettingsDao> settingsDao,
                         const std::shared_ptr<MediaPlayer> mediaPlayer) 
    : Command("help", "Show help page", stationsDao, settingsDao, mediaPlayer)
    , m_commandInterpreter(commandInterpreter) {
}

void HelpCommand::execute(const std::vector<std::string>& args) {
    LOG(plog::info) << "cora - listen to internet radio stations";
    m_commandInterpreter->showCommands();
}