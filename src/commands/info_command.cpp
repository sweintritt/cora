#include "commands/info_command.hpp"

#include <plog/Log.h>

#include <iostream>

#include "utils.hpp"

InfoCommand::InfoCommand(const std::shared_ptr<StationsDao> stationsDao, 
                         const std::shared_ptr<SettingsDao> settingsDao,
                         const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("info", "Show information about a station, given by id", stationsDao, settingsDao, mediaPlayer) {
}

void InfoCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    if (m_cli.getResidualValues().empty()) {
        LOG(plog::error) << "No id given. See 'cora info --help' for more information.";
        return;
    }

    // First entry is the command
    const std::string idStr = m_cli.getResidualValues()[1];
    m_stationsDao->open(m_cli.getValue('f', getDefaultFile()));

    LOG(plog::debug) << "parsing id: " << idStr;
    const long id = std::stol(idStr);
    const auto station = m_stationsDao->findById(id);
    if (station != nullptr) {
        LOG(plog::info) << "      station: " << station->getName();
        LOG(plog::info) << "        genre: " << station->getGenre();
        LOG(plog::info) << "      country: " << station->getCountry();
        LOG(plog::info) << "     language: " << station->getLanguage();
        LOG(plog::info) << "  description: " << formatDescription(station->getDescription());
        LOG(plog::info) << "     added by: " << station->getAddedBy();
        for (unsigned int i = 0; i < station->getUrls().size(); ++i) {
            LOG(plog::info) << "       url[" << i << "]: " << station->getUrls()[i];
        }
    } else {
        LOG(plog::warning) << "No station found for id:" << id;
    }

    m_stationsDao->close();
}

std::string InfoCommand::formatDescription(const std::string& description) const {
    if (description.size() < 60) {
        return description;
    }

    std::string result{""};
    const std::string breakAndInline = "\n               ";
    int lineLength = 0;
    for (unsigned int i = 0; i < description.size(); ++i) {
        if (description[i] == ' ' && lineLength > 80) {
            result += breakAndInline;
            lineLength = 0;
        } else {
            result += description[i];
            ++lineLength;
        }
    }

    return result;
}