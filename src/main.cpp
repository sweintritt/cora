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
#include "utils.hpp"
#include "cli/cli.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "commands/stop_command.hpp"
#include "logging/message_only_formatter.hpp"

// const std::string DB_FILE = "~/.cora.sqlite";
const std::string DB_FILE = ":memory:";

CommandInterpreter commaneInterpreter;
std::shared_ptr<StationsDao> stationsDao;
std::shared_ptr<MediaPlayer> mediaPlayer;

void setupCli(const std::shared_ptr<StationsDao> stationsDao, const std::shared_ptr<MediaPlayer> mediaPlayer) {
    commaneInterpreter.add(std::unique_ptr<Command>(new ListCommand(stationsDao, mediaPlayer)));
    commaneInterpreter.add(std::unique_ptr<Command>(new PlayCommand(stationsDao, mediaPlayer)));
    commaneInterpreter.add(std::unique_ptr<Command>(new StopCommand(stationsDao, mediaPlayer)));
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

    Station cinemix;
    cinemix.setName("Cinemix");
    cinemix.setAuthor(Author::USER);
    cinemix.setGenre("Soundtracks");
    cinemix.setLanguage("English");
    cinemix.setCountry("");
    cinemix.setDescription("The Spirit of Soundtracks");
    cinemix.addUrl("https://streamingv2.shoutcast.com/CINEMIX");
    stationsDao->save(cinemix);

    Station bigRalternative;
    bigRalternative.setName("Big R Radio - 90s Alternative Rock");
    bigRalternative.setAuthor(Author::USER);
    bigRalternative.setGenre("Alternative Rock");
    bigRalternative.setLanguage("English");
    bigRalternative.setCountry("United States of America");
    bigRalternative.setDescription("The Spirit of Soundtracks");
    bigRalternative.addUrl("http://bigrradio.cdnstream1.com/5187_128");
    stationsDao->save(bigRalternative);
}

int run () {
    LOG(plog::debug) << "Initializing player";
    mediaPlayer = std::make_shared<QtMediaPlayer>();
    LOG(plog::debug) << "Opening database file " << DB_FILE;
    stationsDao = std::make_shared<SqliteStationsDao>();
    stationsDao->open(DB_FILE);
    addStations();
    LOG(plog::debug) << "Initializing command interpreter";
    setupCli(stationsDao, mediaPlayer);

    std::string input{""};
    while (input.compare("quit")) {
        std::cout << "cora> ";
        std::getline(std::cin, input);
        LOG(plog::debug) << "input: '" << input << "'";

        if (!input.empty()) {
            const std::vector<std::string> args = split(input);
            const std::string cmd = args[0];
            LOG(plog::debug) << "args.size(): " << args.size();

            try {
                commaneInterpreter.execute(args);
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
    }

    stationsDao->close();
    return EXIT_SUCCESS;
}

void configureLogger(const bool debug) {
    if (debug) {
        static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &consoleAppender);
    } else {
        static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
        plog::init(plog::info, &consoleAppender);
    }

}

int main(int argc, char* argv[]) {
    Cli cli;
    cli.addOption('d', "debug", false, "Setup debug mode.");
    cli.addOption('h', "help", false, "Show help page.");
    cli.parse(argc, argv);

    configureLogger(cli.hasOption('d'));

    if (cli.hasOption('h')) {
        std::cout << cli.usage("cora");
        return EXIT_SUCCESS;
    }

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

