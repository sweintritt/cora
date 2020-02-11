#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

#include "qt_media_player.hpp"

const std::string DEFAULT_URL = "http://webstream.gong971.de/gong971";

void play(const std::string& url) {
    std::shared_ptr<MediaPlayer> player = std::make_shared<QtMediaPlayer>();
    player->setUrl(url);
    player->setVolume(50);
    player->play();
    std::cout << "Press enter to stop and quit";
    while (std::cin.get() != '\n') {}
    player->stop();
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

