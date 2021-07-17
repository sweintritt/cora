#include "commands/play_command.hpp"

#include <plog/Log.h>

#include <iostream>

#include "utils.hpp"

PlayCommand::PlayCommand() : Command("play", "Play a station, given by id") {
}

PlayCommand::~PlayCommand() { }

void PlayCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    m_cli.parse(args);
    if (m_cli.getResidualValues().empty()) {
        LOG(plog::error) << "No id given. See 'cora play --help' for more information.";
        return;
    }

    // First entry is the command
    const std::string idAndUrl = m_cli.getResidualValues()[1];
    auto mediaPlayer = createPlayer();
    auto stationsDao = createStationsDao();
    stationsDao->open(m_cli.getValue('f', getDefaultFile()));

    const std::vector<std::string> values = split(idAndUrl, ':');
    const long id = std::stol(values[0]);
    const auto station = stationsDao->findById(id);
    if (station != nullptr) {
        LOG(plog::info) << "playing " << station->getName();
        std::string url;
        if (values.size() > 1) {
            const long index = std::stol(values[1]);

            if (station->getUrls().size() < index) {
                LOG(plog::warning) << "Only " << station->getUrls().size() << " found. Index " << index << " is invalid";
                LOG(plog::info) << "Using default index 0";
                url = station->getUrls()[0];
            } else {
                url = station->getUrls()[index];
            }
        } else {
            url = station->getUrls()[0];
        }

        LOG(plog::debug) << "url: " << url;
        mediaPlayer->setUrl(url);
        mediaPlayer->play();

        LOG(plog::info) << "Press enter to stop playing";
        std::cin.get();
        mediaPlayer->stop();
    } else {
        LOG(plog::warning) << "No station found for id:" << id;
    }
}