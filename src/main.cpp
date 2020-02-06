#include <stdio.h>
#include <stdlib.h>

#include <QMediaPlayer>
#include <QUrl>

#include <string>
#include <chrono>
#include <thread>
#include <iostream>

const std::string DEFAULT_URL = "http://webstream.gong971.de/gong971";

void play(const std::string& url) {
    QMediaPlayer player;
    player.setMedia(QUrl(QString::fromStdString(url)));
    player.setVolume(50);
    player.play();
    std::cout << "Press enter to stop and quit";
    while (std::cin.get() != '\n') {}
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

