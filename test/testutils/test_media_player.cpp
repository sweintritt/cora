
#include "testutils/test_media_player.hpp"

void TestMediaPlayer::setUrl(const std::string &url) {
    m_url = url;
}

void TestMediaPlayer::play() {
    m_playing = true;
}

void TestMediaPlayer::pause() {
    m_playing = false;
}

void TestMediaPlayer::stop() {
    m_playing = false;
}

void TestMediaPlayer::mute(const bool mute) {
    m_mute = mute;
}

void TestMediaPlayer::setVolume(const int level) {
    m_volume = level;
}
