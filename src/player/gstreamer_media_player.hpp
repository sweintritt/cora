#pragma once

#include "player/media_player.hpp"

#include <gst/gst.h>

class GstreamerMediaPlayer : public MediaPlayer {

    public:
        GstreamerMediaPlayer();

        virtual ~GstreamerMediaPlayer();

        virtual void setUrl(const std::string& url) override;

        virtual void play() override;

        virtual void pause() override;

        virtual void stop() override;

        virtual void mute(const bool mute) override;

        virtual void setVolume(const int level) override;

    private:
        struct State {
            std::string nowPlaying;
            bool isLive;
            GstElement *pipeline;
            GMainLoop *loop;
        };

        QMediaPlayer* player;

    public slots:
        void onMediaChanged(const QMediaContent &media);
};
