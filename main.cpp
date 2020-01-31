#include <stdio.h>
#include <stdlib.h>
#include <vlc/vlc.h>

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

const std::string DEFAULT_URL = "http://webstream.gong971.de/gong971";

void play(const std::string& url) {
    /* Load the VLC engine */
    libvlc_instance_t* inst = libvlc_new (0, NULL);

    /* Create a new item */
    libvlc_media_t* m = libvlc_media_new_location (inst, url.c_str());
    /* Create a media player playing environement */
    libvlc_media_player_t* mp = libvlc_media_player_new_from_media (m);
    /* No need to keep the media now */
    libvlc_media_release (m);

    libvlc_media_player_play (mp);

    std::cout << "Press enter to stop and quit";
    while (std::cin.get() != '\n') {}

    libvlc_media_player_stop (mp);
    libvlc_media_player_release (mp);
    libvlc_release (inst);

}

int main(int argc, char* argv[]) {
    std::string url;

    if (argc > 1) {
        url = std::string{argv[1]};
    } else {
        url = DEFAULT_URL;
    }

    play(url);
    return EXIT_SUCCESS;
}

