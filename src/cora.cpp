#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "cora.hpp"
#include "qt_media_player.hpp"
#include "sqlite_stations_dao.hpp"
#include "utils.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/find_command.hpp"
#include "commands/import_command.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "commands/stop_command.hpp"
#include "logging/message_only_formatter.hpp"

Cora::Cora()
    : m_cli("cora", "listen to internet radio stations")
    , m_commandInterpreter()
    , m_stationsDao()
    , m_mediaPlayer() {

    m_cli.addOption('d', "debug", false, "Setup debug mode.");
    m_cli.addOption('f', "file", true, "Database file.");
    m_cli.addOption('h', "help", false, "Show help page.");
}

Cora::~Cora() {}

void Cora::init(int argc, char* argv[]) {
    m_cli.parse(argc, argv);

    configureLogger(m_cli.hasOption('d'));

    if (m_cli.hasOption('h')) {
        std::cout << m_cli.usage();
        return;
    }

    std::string file;
    if (m_cli.hasOption('f')) {
        file = m_cli.getValue('f');
    } else {
        file = getDefaultFile();
    }

    LOG(plog::debug) << "Initializing player";
    m_mediaPlayer = std::make_shared<QtMediaPlayer>();
    LOG(plog::debug) << "Opening database file " << file;
    m_stationsDao = std::make_shared<SqliteStationsDao>();
    m_stationsDao->open(file);
    // addStations();
    LOG(plog::debug) << "Initializing command interpreter";
    m_commandInterpreter.add(std::unique_ptr<Command>(new FindCommand(m_stationsDao, m_mediaPlayer)));
    m_commandInterpreter.add(std::unique_ptr<Command>(new ImportCommand(m_stationsDao, m_mediaPlayer)));
    m_commandInterpreter.add(std::unique_ptr<Command>(new ListCommand(m_stationsDao, m_mediaPlayer)));
    m_commandInterpreter.add(std::unique_ptr<Command>(new PlayCommand(m_stationsDao, m_mediaPlayer)));
    m_commandInterpreter.add(std::unique_ptr<Command>(new StopCommand(m_stationsDao, m_mediaPlayer)));
}

std::string Cora::getDefaultFile() {
    // TODO Get user name and build default path
    // return ":memory:";
    return "./cora.sqlite";
}

void Cora::addStations() {
    Station radioGong;
    radioGong.setName("Radio Gong");
    radioGong.setAuthor(Author::USER);
    radioGong.setGenre("Rock");
    radioGong.setLanguage("German");
    radioGong.setCountry("Germany");
    radioGong.setDescription("Der Rocksender");
    radioGong.addUrl("http://webstream.gong971.de/gong971");
    m_stationsDao->save(radioGong);

    Station cinemix;
    cinemix.setName("Cinemix");
    cinemix.setAuthor(Author::USER);
    cinemix.setGenre("Soundtracks");
    cinemix.setLanguage("English");
    cinemix.setCountry("");
    cinemix.setDescription("The Spirit of Soundtracks");
    cinemix.addUrl("https://streamingv2.shoutcast.com/CINEMIX");
    m_stationsDao->save(cinemix);

    Station bigRalternative;
    bigRalternative.setName("Big R Radio - 90s Alternative Rock");
    bigRalternative.setAuthor(Author::USER);
    bigRalternative.setGenre("Alternative Rock");
    bigRalternative.setLanguage("English");
    bigRalternative.setCountry("United States of America");
    bigRalternative.setDescription("90s Alternative Rock");
    bigRalternative.addUrl("http://bigrradio.cdnstream1.com/5187_128");
    m_stationsDao->save(bigRalternative);
}

void Cora::run () {
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
                m_commandInterpreter.execute(args);
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

    m_stationsDao->close();
}

void Cora::configureLogger(const bool debug) {
    if (debug) {
        static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &consoleAppender);
        LOG(plog::debug) << "Debug logging configured";
    } else {
        static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
        plog::init(plog::info, &consoleAppender);
    }
}