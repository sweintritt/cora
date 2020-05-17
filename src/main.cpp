#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

#include "qt_media_player.hpp"
#include "sqlite_stations_dao.hpp"

const std::string DEFAULT_URL = "http://webstream.gong971.de/gong971";

void play(const std::string& url) {
    //std::shared_ptr<MediaPlayer> player = MediaPlayer::getPayer();
    auto player = std::make_shared<QtMediaPlayer>();
    player->setUrl(url);
    player->setVolume(50);
    player->play();
    std::cout << "Press enter to stop and quit";
    while (std::cin.get() != '\n') {}
    player->stop();
}

void open() {
    try {
        auto stationsDao = std::make_shared<SqliteStationsDao>();
        stationsDao->open("stations.sqlite");
        stationsDao->close();
    } catch (const std::exception& error) {
        LOG(ERROR) << error.what();
    } catch (const std::string& error) {
        LOG(ERROR) << error;
    } catch (const char* error) {
        LOG(ERROR) << error;
    } catch (...) {
        LOG(ERROR) << "error of unknown type";
    }
}

int main(int argc, char* argv[]) {
    LOG(INFO) << "Starting cora";
    std::string url;

    if (argc > 1) {
        url = std::string{argv[1]};
    } else {
        url = DEFAULT_URL;
    }

    // MediaPlayer::setPayer(std::make_shared<QtMediaPlayer>());
    // play(url);
    open();
    return EXIT_SUCCESS;
}

