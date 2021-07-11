#pragma once

#include "player/media_player.hpp"

#include <memory>
#include <thread>
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

        State state;
        GstBus* bus;
        std::shared_ptr<std::thread> playThread;

        void run();
        void cleanup();
        static void handleMessage(GstBus *bus, GstMessage *msg, State *state);
        static void printError(GstMessage *msg);
        static void printTitle(const GstTagList *list, const gchar *tag, gpointer data);
};
