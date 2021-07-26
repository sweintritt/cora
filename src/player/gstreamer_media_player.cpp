
#include "player/gstreamer_media_player.hpp"

#include <plog/Log.h>
#include <gst/gst.h>

GstreamerMediaPlayer::GstreamerMediaPlayer() : state(), bus(nullptr), playThread() {
    gst_init(0, NULL);
}

GstreamerMediaPlayer::~GstreamerMediaPlayer() {
    cleanup();
}

void GstreamerMediaPlayer::cleanup () {
    if (state.loop != nullptr) {
        g_main_loop_quit(state.loop);
        playThread->join();
        g_main_loop_unref (state.loop);
    }

    if (bus != nullptr) {
        gst_object_unref (bus);
    }

    if (state.pipeline != nullptr) {
        gst_element_set_state (state.pipeline, GST_STATE_NULL);
        gst_object_unref (state.pipeline);
    }
}

void GstreamerMediaPlayer::setUrl(const std::string &url) {
    cleanup();
    const std::string uri = "playbin uri=" + url;
    state.pipeline = gst_parse_launch(uri.c_str(), NULL);
    bus = gst_element_get_bus(state.pipeline);
}

void GstreamerMediaPlayer::play() {
    playThread = std::make_shared<std::thread>(&GstreamerMediaPlayer::run, this);
}

void GstreamerMediaPlayer::run() {
    const GstStateChangeReturn ret = gst_element_set_state(state.pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        LOG(plog::error) << "Unable to set the pipeline to the playing state";
        gst_object_unref(state.pipeline);
        return;
    } else if (ret == GST_STATE_CHANGE_NO_PREROLL) {
        state.isLive = true;
    }

    state.loop = g_main_loop_new(NULL, FALSE);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message", G_CALLBACK(GstreamerMediaPlayer::handleMessage), &state);
    g_main_loop_run(state.loop);
}


void GstreamerMediaPlayer::pause() {
    if (state.pipeline != nullptr) {
        gst_element_set_state(state.pipeline, GST_STATE_PAUSED);
    }
}

void GstreamerMediaPlayer::stop() {
    if (state.pipeline != nullptr) {
        gst_element_set_state(state.pipeline, GST_STATE_PAUSED);
    }
}

void GstreamerMediaPlayer::mute(const bool mute) {
    LOG(plog::warning) << "Not implemented";
}

void GstreamerMediaPlayer::setVolume(const int level) {
    LOG(plog::warning) << "Not implemented";
}

void GstreamerMediaPlayer::printTitle(const GstTagList *list, const gchar *tag, gpointer data) {
    const std::string titleTag = "title";
    const int num = gst_tag_list_get_tag_size(list, tag);

    for (int i = 0; i < num; ++i) {
        const GValue *val = gst_tag_list_get_value_index(list, tag, i);
        if (G_VALUE_HOLDS_STRING(val) && (titleTag.compare(tag) == 0)) {
            const std::string title{g_value_get_string(val)};
            State* state = (State*) data;

            if (state->nowPlaying.compare(title) != 0) {
                LOG(plog::info) << title;
                state->nowPlaying = title;
            }
        }
    }
}

void GstreamerMediaPlayer::printError(GstMessage *msg) {
    GError *err;
    gchar *debug;

    gst_message_parse_error(msg, &err, &debug);
    LOG(plog::error) << "ERROR: " << err->message;
    g_error_free(err);
    g_free(debug);
}

void GstreamerMediaPlayer::handleMessage(GstBus *bus, GstMessage *msg, State *state) {
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR: {
        printError(msg);
        gst_element_set_state(state->pipeline, GST_STATE_READY);
        g_main_loop_quit(state->loop);
        break;
    }
    case GST_MESSAGE_TAG: {
        GstTagList *tags = NULL;
        gst_message_parse_tag(msg, &tags);
        gst_tag_list_foreach(tags, printTitle, state);
        gst_tag_list_unref(tags);
        break;
    }
    case GST_MESSAGE_EOS: /* end-of-stream */
        gst_element_set_state(state->pipeline, GST_STATE_READY);
        g_main_loop_quit(state->loop);
        break;
    case GST_MESSAGE_BUFFERING: {
        if (state->isLive) {
            break;
        }

        gint percent = 0;
        gst_message_parse_buffering(msg, &percent);
        if (percent < 100) {
            gst_element_set_state(state->pipeline, GST_STATE_PAUSED);
        } else {
            gst_element_set_state(state->pipeline, GST_STATE_PLAYING);
        }
        break;
    }
    case GST_MESSAGE_CLOCK_LOST:
        gst_element_set_state(state->pipeline, GST_STATE_PAUSED);
        gst_element_set_state(state->pipeline, GST_STATE_PLAYING);
        break;
    default:
        break;
    }
}
