#include "commands/find_command.hpp"

#include <plog/Log.h>

#include <regex>

const std::regex FIND_REGEX{"[- a-zA-Z0-9]*"};

FindCommand::FindCommand(const std::shared_ptr<StationsDao> stationsDao, 
                         const std::shared_ptr<SettingsDao> settingsDao,
                         const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("find", "Find stations by name, genre and country.", stationsDao, settingsDao, mediaPlayer) {
    m_cli.addOption('n', "name", true, "Search by name");
    m_cli.addOption('g', "genre", true, "Search by genre");   
    m_cli.addOption('c', "country", true, "Search by country");   
}

void FindCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }
    // FIXME c?
    if (!std::regex_match(m_cli.getValue('n'), FIND_REGEX)) {
        LOG(plog::error) << "Invalid search value. See 'cora find --help' for more information.";
        return;
    }

    if (!std::regex_match(m_cli.getValue('n'), FIND_REGEX)) {
        LOG(plog::error) << "Invalid value for name. See 'cora find --help' for more information.";
        return;
    }

    if (!std::regex_match(m_cli.getValue('g'), FIND_REGEX)) {
        LOG(plog::error) << "Invalid value for genre. See 'cora find --help' for more information.";
        return;
    }

    m_stationsDao->open(m_cli.getValue('f', getDefaultFile()));
    const auto ids = m_stationsDao->find(m_cli.getValue('n', ""), m_cli.getValue('g', ""), m_cli.getValue('c', ""));

    for (const auto& id : ids) {
        const auto station = m_stationsDao->findById(id);
        if (station != nullptr) {
            LOG(plog::info) << "id:" << std::to_string(station->getId())
                << "\", name:\"" << station->getName()
                << "\", genre:\"" << station->getGenre()
                << "\", country:\"" << station->getCountry() << "\"";
        } else {
            LOG(plog::warning) << "no station found for id:" << id;
        }
    }

    m_stationsDao->close();
}