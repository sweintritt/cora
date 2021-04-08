#include "commands/list_command.hpp"

#include <plog/Log.h>

ListCommand::ListCommand() : Command("list", "List all available stations") {
}

ListCommand::~ListCommand() { }

void ListCommand::execute(const std::vector<std::string>& args) {
    // TODO Move into command class
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    auto stationsDao = createStationsDao();
    if (m_cli.hasValue('f')) {
        stationsDao->open(m_cli.getValue('f'));
    } else {
        stationsDao->open(getDefaultFile());
    }

    const auto ids = stationsDao->getAllIds();

    for (const auto& id : ids) {
        const auto station = stationsDao->findById(id);
        if (station != nullptr) {
            LOG(plog::info) << "id:" << std::to_string(station->getId())
                << ", name:" << station->getName()
                << ", genre:" << station->getGenre()
                << ", country:" << station->getCountry()
                << ", language:" << station->getLanguage();
        } else {
            LOG(plog::warning) << "no station found for id:" << id;
        }
    }
}