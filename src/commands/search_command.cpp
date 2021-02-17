#include "commands/search_command.hpp"

SearchCommand::SearchCommand(const std::shared_ptr<StationsDao> stationsDao,
    const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("search", "search for radio staions", stationsDao, mediaPlayer) {
        m_cli.addOption('h', "help", false, "Show help page");
        m_cli.addOption('n', "name", true, "Name of the radio stations");
        m_cli.addOption('g', "genre", true, "Gerne of the radio stations");
        m_cli.addOption('c', "country", true, "Country of the radio stations");
        m_cli.addOption('l', "language", true, "Language of the radio stations");
        m_cli.addOption('x', "limit", true, "Limit the number of the returned results. Default is 100.");
}

SearchCommand::~SearchCommand() { }

void SearchCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    const std::string name = (m_cli.hasValue('n')) ? m_cli.getValue('n') : "";
    const std::string genre = (m_cli.hasValue('g')) ? m_cli.getValue('g') : "";
    const std::string country = (m_cli.hasValue('c')) ? m_cli.getValue('c') : "";
    const std::string language = (m_cli.hasValue('l')) ? m_cli.getValue('l') : "";
    const int limit = (m_cli.hasValue('x')) ? std::stoi(m_cli.getValue('x')) : 100;

    const auto stations = m_stationsDao->find(name, genre, country, language, limit);
    for (const auto& station : stations) {
        LOG(plog::info) << std::to_string(station.getId()) << " - " << station.getName() << " - " << station.getDescription();
    }
}