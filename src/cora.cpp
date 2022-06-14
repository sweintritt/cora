#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <thread>
#include <iostream>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "cora.hpp"
#include "utils.hpp"
#include "version.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/find_command.hpp"
#include "commands/help_command.hpp"
#include "commands/info_command.hpp"
#include "commands/import_command.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "commands/version_command.hpp"
#include "db/stations_dao.hpp"
#include "db/settings_dao.hpp"
#include "logging/message_only_formatter.hpp"
#include "player/gstreamer_media_player.hpp"

Cora::Cora() {
    m_stationsDao = std::make_shared<StationsDao>();
    m_settingsDao = std::make_shared<SettingsDao>();
    m_mediaPlayer = std::make_shared<GstreamerMediaPlayer>();
    m_commandInterpreter = std::make_shared<CommandInterpreter>();
    m_commandInterpreter->add(std::unique_ptr<Command>(new InfoCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new ImportCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new HelpCommand(m_commandInterpreter, m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new ListCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new PlayCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new FindCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
    m_commandInterpreter->add(std::unique_ptr<Command>(new VersionCommand(m_stationsDao, m_settingsDao, m_mediaPlayer)));
}

void Cora::run(const std::vector<std::string>& args) {
    configureLogger();
    LOG(plog::debug) << "Starting cora v" << CORA_VERSION;

    if (args.size() < 2) {
        LOG(plog::warning) << "No command given. Try 'cora help' for more information.";
        return;
    }

    runCommand(args);
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
        m_commandInterpreter->execute(args);
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