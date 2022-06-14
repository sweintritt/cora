#include "commands/version_command.hpp"

#include "version.hpp"

#include <plog/Log.h>

VersionCommand::VersionCommand(const std::shared_ptr<StationsDao> stationsDao, 
                               const std::shared_ptr<SettingsDao> settingsDao,
                               const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("version", "Show the version", stationsDao, settingsDao, mediaPlayer) { }

void VersionCommand::execute(const std::vector<std::string>& args) {
    LOG(plog::info) << "Cora v" << CORA_VERSION;
}