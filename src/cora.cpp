#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "cora.hpp"
#include "utils.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/import_command.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "logging/message_only_formatter.hpp"

Cora::Cora() : m_commandInterpreter() {
    m_commandInterpreter.add(std::unique_ptr<Command>(new ImportCommand()));
    m_commandInterpreter.add(std::unique_ptr<Command>(new ListCommand()));
    m_commandInterpreter.add(std::unique_ptr<Command>(new PlayCommand()));
}

Cora::~Cora() {}

void Cora::run(int argc, char* argv[]) {
    configureLogger();

    if (argc < 2) {
        LOG(plog::debug) << "argc = " << argc;
        LOG(plog::warning) << "No command given. Try 'cora help' for more information.";
        return;
    }

    if (std::string{argv[1]}.compare("help") == 0) {
        // TODO Show commands
        std::cout << "TODO Show help" << std::endl;
        return;
    }

    if (!m_commandInterpreter.hasCommand(argv[1])) {
        LOG(plog::warning) << "Unknown command '" << argv[1] << "'. Try 'cora help' for more information.";
        return;
    }

    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    runCommand(args);
}

void Cora::showUsage() {
    LOG(plog::info) << "cora - listen to internet radio stations";
    // TODO get commands and usages
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
    //m_stationsDao->save(radioGong);

    Station cinemix;
    cinemix.setName("Cinemix");
    cinemix.setAuthor(Author::USER);
    cinemix.setGenre("Soundtracks");
    cinemix.setLanguage("English");
    cinemix.setCountry("");
    cinemix.setDescription("The Spirit of Soundtracks");
    cinemix.addUrl("https://streamingv2.shoutcast.com/CINEMIX");
    //m_stationsDao->save(cinemix);

    Station bigRalternative;
    bigRalternative.setName("Big R Radio - 90s Alternative Rock");
    bigRalternative.setAuthor(Author::USER);
    bigRalternative.setGenre("Alternative Rock");
    bigRalternative.setLanguage("English");
    bigRalternative.setCountry("United States of America");
    bigRalternative.setDescription("90s Alternative Rock");
    bigRalternative.addUrl("http://bigrradio.cdnstream1.com/5187_128");
    //m_stationsDao->save(bigRalternative);
}

void Cora::configureLogger() const {
    #ifndef NDEBUG
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    LOG(plog::debug) << "Debug logging configured";
    #else
    static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
    plog::init(plog::info, &consoleAppender);
    #endif
}

void Cora::runCommand(const std::vector<std::string>& args) {
    const std::string cmd = args[0];

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