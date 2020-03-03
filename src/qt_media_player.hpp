#pragma once

#include "media_player.hpp"

#include <QMediaPlayer>

class QtMediaPlayer : public MediaPlayer {

    public:
        QtMediaPlayer();

        virtual ~QtMediaPlayer();

        virtual void setUrl(const std::string& url) override;

        virtual void play() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void mute(const bool mute) override;

        virtual void setVolume(const int level) override;

    private:
        QMediaPlayer* player;
};
