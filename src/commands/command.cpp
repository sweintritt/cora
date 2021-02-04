#include "command.hpp"

Command::Command(const std::string& name,
                 const std::shared_ptr<StationsDao> stationsDao,
                 const std::shared_ptr<MediaPlayer> mediaPlayer)
    : m_name(name)
    , m_stationsDao(stationsDao)
    , m_mediaPlayer(mediaPlayer) {}

Command::~Command() { }


const std::string& Command::getName() const {
    return m_name;
}
