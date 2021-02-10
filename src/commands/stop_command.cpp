#include "commands/stop_command.hpp"

StopCommand::StopCommand(const std::shared_ptr<StationsDao> stationsDao,
    const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("stop", "stop playing", stationsDao, mediaPlayer) {
}

StopCommand::~StopCommand() { }

void StopCommand::execute(const std::vector<std::string>& args) {
    m_mediaPlayer->stop();
}