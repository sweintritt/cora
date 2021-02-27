#include "commands/list_command.hpp"

ListCommand::ListCommand(const std::shared_ptr<StationsDao> stationsDao,
    const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("list", "list available stations", stationsDao, mediaPlayer) {
}

ListCommand::~ListCommand() { }

void ListCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);
    const auto ids = m_stationsDao->getAllIds();

    for (const auto& id : ids) {
        const auto station = m_stationsDao->findById(id);
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