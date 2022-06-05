#include "command.hpp"

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "stations_dao.hpp"
#include "logging/message_only_formatter.hpp"
#include "player/gstreamer_media_player.hpp"

Command::Command(const std::string& name,
                 const std::string& description)
    : m_name(name)
    , m_cli(name, description) {
        m_cli.addOption('h', "help", false, "Show help page");
        m_cli.addOption('f', "file", true, "Database file. Default is " + getDefaultFile());
    }

Command::~Command() { }

const std::string& Command::getName() const {
    return m_name;
}

std::string Command::getDescription() const {
    return m_cli.description();
}

std::string Command::getUsage() const {
    return m_cli.usage();
}

const std::string Command::getDefaultFile() const {
    const std::string username{getenv("USER")};
    return "/home/" + username + "/.cora.sqlite";
}

std::shared_ptr<MediaPlayer> Command::createPlayer() const {
    return std::make_shared<GstreamerMediaPlayer>();
}

std::shared_ptr<StationsDao> Command::createStationsDao() const {
    return std::make_shared<StationsDao>();
}