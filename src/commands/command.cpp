#include "command.hpp"

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>

#include "logging/message_only_formatter.hpp"
#include "player/gstreamer_media_player.hpp"

Command::Command(const std::string& name,
                 const std::string& description, 
                 const std::shared_ptr<StationsDao> stationsDao, 
                 const std::shared_ptr<SettingsDao> settingsDao,
                 const std::shared_ptr<MediaPlayer> mediaPlayer)
    : m_name(name)
    , m_cli(name, description) 
    , m_stationsDao(stationsDao) 
    , m_settingsDao(settingsDao)
    , m_mediaPlayer(mediaPlayer) {

        m_cli.addOption('h', "help", false, "Show help page");
        m_cli.addOption('f', "file", true, "Database file. Default is " + getDefaultFile());

    }

const std::string& Command::getName() const {
    return m_name;
}

std::string Command::getDescription() const {
    return m_cli.description();
}

std::string Command::getUsage() const {
    return m_cli.usage();
}

std::string Command::getDefaultFile() const {
    const std::string username{getenv("USER")};
    return "/home/" + username + "/.cora.sqlite";
}
