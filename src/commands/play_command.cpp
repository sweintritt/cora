#include "commands/play_command.hpp"

PlayCommand::PlayCommand(const std::shared_ptr<StationsDao> stationsDao,
    const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("play", "play a station given by id", stationsDao, mediaPlayer) {
}

PlayCommand::~PlayCommand() { }

void PlayCommand::execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        LOG(plog::error) << "no id given";
        return;
    }

    const long id = std::stol(args[1]);
    const auto station = m_stationsDao->findById(id);
    if (station != nullptr) {
        LOG(plog::info) << "playing " << station->getName();
        LOG(plog::debug) << "url: " << station->getUrls()[0];
        m_mediaPlayer->setUrl(station->getUrls()[0]);
        m_mediaPlayer->setVolume(50);
        m_mediaPlayer->play();
    } else {
        LOG(plog::warning) << "no station found for id:" << id;
    }
}