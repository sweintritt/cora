#include "player/qt_media_player.hpp"

#include <plog/Log.h>

#include <QUrl>

QtMediaPlayer::QtMediaPlayer() : player(new QMediaPlayer) {
    connect(player, &QMediaPlayer::currentMediaChanged, this, &QtMediaPlayer::onMediaChanged);
}

QtMediaPlayer::~QtMediaPlayer() {

}

void QtMediaPlayer::setUrl(const std::string& url) {
    player->setMedia(QUrl(QString::fromStdString(url)));
}

void QtMediaPlayer::play() {
    player->play();
}

void QtMediaPlayer::pause() {
    player->pause();
}

void QtMediaPlayer::stop() {
    player->stop();
}

void QtMediaPlayer::mute(const bool mute) {
    player->setMuted(mute);
}

void QtMediaPlayer::setVolume(const int level) {
    player->setVolume(level);
}

void QtMediaPlayer::onMediaChanged(const QMediaContent &media) {
    LOG(plog::debug) << "Received currentMediaChanged singal";
}