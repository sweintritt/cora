#include "command.hpp"

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "qt_media_player.hpp"
#include "sqlite_stations_dao.hpp"
#include "logging/message_only_formatter.hpp"

Command::Command(const std::string& name,
                 const std::string& description)
    : m_name(name)
    , m_cli(name, description) {
        m_cli.addOption('h', "help", false, "Show help page");
        m_cli.addOption('d', "debug", false, "Setup debug mode.");
        m_cli.addOption('f', "file", true, "Database file.");
    }

Command::~Command() { }

const std::string& Command::getName() const {
    return m_name;
}

std::string Command::getUsage() const {
    return m_cli.usage();
}

const std::string Command::getDefaultFile() const {
    const std::string username{getenv("USER")};
    return "/home/" + username + "/.cora.sqlite";
}

void Command::configureLogger(const bool debug) const {
    if (debug) {
        static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
        plog::init(plog::debug, &consoleAppender);
        LOG(plog::debug) << "Debug logging configured";
    } else {
        static plog::ConsoleAppender<MessageOnlyFormatter> consoleAppender;
        plog::init(plog::info, &consoleAppender);
    }
}

std::shared_ptr<MediaPlayer> Command::createPlayer() const {
    return std::make_shared<QtMediaPlayer>();
}

std::shared_ptr<StationsDao> Command::createStationsDao() const {
    return std::make_shared<SqliteStationsDao>();
}