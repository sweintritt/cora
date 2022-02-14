#include "commands/find_command.hpp"

#include <plog/Log.h>

#include <regex>

const std::regex FIND_REGEX{"[a-zA-Z0-9]*"};

FindCommand::FindCommand() : Command("find", "Find stations by a given search value. Only characters and number are allowed.") {
}

FindCommand::~FindCommand() { }

void FindCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    if (m_cli.getResidualValues().empty()) {
        LOG(plog::error) << "No search value given. See 'cora find --help' for more information.";
        return;
    }

    const std::string value = m_cli.getResidualValues()[1];
    if (!std::regex_match(value, FIND_REGEX)) {
        LOG(plog::error) << "Invalid search value. See 'cora find --help' for more information.";
        return;
    }

    auto stationsDao = createStationsDao();
    stationsDao->open(m_cli.getValue('f', getDefaultFile()));
    const auto ids = stationsDao->find(m_cli.getResidualValues()[1]);

    for (const auto& id : ids) {
        const auto station = stationsDao->findById(id);
        if (station != nullptr) {
            LOG(plog::info) << "id:" << std::to_string(station->getId())
                << "\", name:\"" << station->getName()
                << "\", genre:\"" << station->getGenre()
                << "\", country:\"" << station->getCountry() << "\"";
        } else {
            LOG(plog::warning) << "no station found for id:" << id;
        }
    }
}