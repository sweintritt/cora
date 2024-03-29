#include "commands/list_command.hpp"

#include <plog/Log.h>

ListCommand::ListCommand(const std::shared_ptr<StationsDao> stationsDao, 
                         const std::shared_ptr<SettingsDao> settingsDao,
                         const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("list", "List all available stations", stationsDao, settingsDao, mediaPlayer) {
}

void ListCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    m_stationsDao->open(m_cli.getValue('f', getDefaultFile()));
    const auto ids = m_stationsDao->getAllIds();

    for (const auto& id : ids) {
        const auto station = m_stationsDao->findById(id);
        if (station != nullptr) {
            LOG(plog::info) << "id:" << std::to_string(station->getId())
                << ", name:\"" << station->getName()
                << "\", genre:\"" << station->getGenre()
                << "\", country:\"" << station->getCountry() << "\"";
        } else {
            LOG(plog::warning) << "No station found for id:" << id;
        }
    }

    m_stationsDao->close();
}