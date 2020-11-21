#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "qt_media_player.hpp"
#include "sqlite_stations_dao.hpp"

// const std::string DB_FILE = "~/.cora.sqlite";
const std::string DB_FILE = ":memory:";

std::shared_ptr<StationsDao> stationsDao;
std::shared_ptr<MediaPlayer> mediaPlayer;

void play(const long id) {
    const auto station = stationsDao->findById(id);
    LOG(plog::info) << "playing " << station.getName();
    LOG(plog::debug) << "url: " << station.getUrls()[0];
    mediaPlayer->setUrl(station.getUrls()[0]);
    mediaPlayer->setVolume(50);
    mediaPlayer->play();
}

void stop() {
    mediaPlayer->stop();
}

void addStations() {
    Station radioGong;
    radioGong.setName("Radio Gong");
    radioGong.setAuthor(Author::USER);
    radioGong.setGenre("Rock");
    radioGong.setLanguage("German");
    radioGong.setCountry("Germany");
    radioGong.setDescription("Der Rocksender");
    radioGong.addUrl("http://webstream.gong971.de/gong971");
    stationsDao->save(radioGong);
}

int run () {
    LOG(plog::debug) << "Initializing player";
    mediaPlayer = std::make_shared<QtMediaPlayer>();
    LOG(plog::debug) << "Opening database file " << DB_FILE;
    stationsDao = std::make_shared<SqliteStationsDao>();
    stationsDao->open(DB_FILE);
    addStations();
    
    std::string input{""};
    while (input.compare("quit")) {
        std::cout << "cora> ";
        std::cin >> input;

        try {
            if (input.compare("play") == 0) {
                play(1);
            } else if (input.compare("stop") == 0) {
                stop();
            }
        } catch (const std::exception& error) {
            LOG(plog::error) << error.what();
        } catch (const std::string& error) {
            LOG(plog::error) << error;
        } catch (const char* error) {
            LOG(plog::error) << error;
        } catch (...) {
            LOG(plog::error) << "error of unknown type";
        }
    }

    stationsDao->close();
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    LOG(plog::info) << "Starting cora";
    try {
        return run();
    } catch (const std::exception& error) {
        LOG(plog::error) << error.what();
    } catch (const std::string& error) {
        LOG(plog::error) << error;
    } catch (const char* error) {
        LOG(plog::error) << error;
    } catch (...) {
        LOG(plog::error) << "error of unknown type";
    }
}

