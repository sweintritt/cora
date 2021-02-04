#include "commands/list_command.hpp"

ListCommand::ListCommand(const std::shared_ptr<StationsDao> stationsDao,
    const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("list", stationsDao, mediaPlayer) {
}

ListCommand::~ListCommand() { }

void ListCommand::execute(const std::vector<std::string>& args) {
    const auto ids = m_stationsDao->getAllIds();

    for (const auto& id : ids) {
        const auto station = m_stationsDao->findById(id);
        if (station != nullptr) {
            LOG(plog::info) << std::to_string(station->getId()) << " - " << station->getName() << " - " << station->getDescription();
        } else {
            LOG(plog::warning) << "no station found for id:" << id;
        }
    }
}