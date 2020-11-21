#include "media_player.hpp"

#include <stdexcept>

std::shared_ptr<MediaPlayer> MediaPlayer::PLAYER = nullptr;

MediaPlayer::~MediaPlayer() { }

std::shared_ptr<MediaPlayer> MediaPlayer::getPlayer() {
    return MediaPlayer::PLAYER;
}

void MediaPlayer::setPlayer(const std::shared_ptr<MediaPlayer> player) {
    if (PLAYER != nullptr) {
        throw std::runtime_error("MediaPlayer can only be set once");
    }
    MediaPlayer::PLAYER = player;
}
