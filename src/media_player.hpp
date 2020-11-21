#pragma once

#include <string>
#include <memory>

class MediaPlayer {

    public:
        virtual ~MediaPlayer();

        virtual void setUrl(const std::string& url) = 0;

        virtual void play() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void mute(const bool mute) = 0;

        virtual void setVolume(const int level) = 0;

        static std::shared_ptr<MediaPlayer> getPlayer();

        static void setPlayer(const std::shared_ptr<MediaPlayer>);

    private:
        static std::shared_ptr<MediaPlayer> PLAYER;
};
