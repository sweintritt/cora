#include "command.hpp"

Command::Command(const std::string& name,
                 const std::string& description,
                 const std::shared_ptr<StationsDao> stationsDao,
                 const std::shared_ptr<MediaPlayer> mediaPlayer)
    : m_name(name)
    , m_cli(name, description)
    , m_stationsDao(stationsDao)
    , m_mediaPlayer(mediaPlayer) {}

Command::~Command() { }


const std::string& Command::getName() const {
    return m_name;
}

std::string Command::getUsage() const {
    return m_cli.usage();
}
