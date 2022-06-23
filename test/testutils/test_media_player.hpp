#pragma once

#include "player/media_player.hpp"

#include <memory>
#include <thread>

class TestMediaPlayer : public MediaPlayer {

    public:
        void setUrl(const std::string& url) override;
        void play() override;
        void pause() override;
        void stop() override;
        void mute(const bool mute) override;
        void setVolume(const int level) override;

        std::string m_url;
        bool m_mute;
        bool m_playing;
        int m_volume;
};
