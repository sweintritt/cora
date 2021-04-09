#include "commands/info_command.hpp"

#include <plog/Log.h>

#include <iostream>

#include "utils.hpp"

InfoCommand::InfoCommand() : Command("info", "Show information about a station, given by id") {
}

InfoCommand::~InfoCommand() { }

void InfoCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    const std::string idStr = findId(args);

    if (idStr.empty()) {
        LOG(plog::error) << "No id given. See 'cora info --help' for more information.";
        return;
    }

    m_cli.parse(args);
    auto stationsDao = createStationsDao();
    if (m_cli.hasValue('f')) {
        stationsDao->open(m_cli.getValue('f'));
    } else {
        stationsDao->open(getDefaultFile());
    }

    const long id = std::stol(idStr);
    const auto station = stationsDao->findById(id);
    if (station != nullptr) {
        LOG(plog::info) << "      station: " << station->getName();
        LOG(plog::info) << "        genre: " << station->getGenre();
        LOG(plog::info) << "      country: " << station->getCountry();
        LOG(plog::info) << "     language: " << station->getLanguage();
        LOG(plog::info) << "  description: " << station->getDescription();
        for (int i = 0; i < station->getUrls().size(); ++i) {
            LOG(plog::info) << "       url[" << i << "]: " << station->getUrls()[i];
        }
    } else {
        LOG(plog::warning) << "No station found for id:" << id;
    }
}

std::string InfoCommand::findId(const std::vector<std::string>& args) const {
    for (unsigned int i = 2; i < args.size(); ++i) {
        if (args[i][0] != '-') {
            return args[i];
        }
    }

    return "";
}