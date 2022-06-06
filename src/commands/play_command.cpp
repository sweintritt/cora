#include "commands/play_command.hpp"

#include <plog/Log.h>

#include <iostream>

#include "utils.hpp"

PlayCommand::PlayCommand() : Command("play", "Play a station, given by id") {
}

void PlayCommand::execute(const std::vector<std::string>& args) {
    m_cli.parse(args);

    if (m_cli.hasOption('h')) {
        LOG(plog::info) << m_cli.usage();
        return;
    }

    if (m_cli.getResidualValues().empty()) {
        LOG(plog::error) << "No id given. See 'cora play --help' for more information.";
        return;
    }

    // First entry is the command
    const std::string idAndUrl = m_cli.getResidualValues()[1];    
    const std::vector<std::string> values = split(idAndUrl, ':');
    std::shared_ptr<Station> station;
    auto stationsDao = createStationsDao();
    stationsDao->open(m_cli.getValue('f', getDefaultFile()));

    LOG(plog::debug) << "checking: " << values[0];
    if (values[0].compare("random") == 0) {
        station = stationsDao->getRandom();
    } else {
        const long id = std::stol(values[0]);
        station = stationsDao->findById(id);
    }

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

        play(url);
    } else {
        LOG(plog::warning) << "No station found for id:" << values[0];
    }
}

void PlayCommand::play(const std::string& url) {
    auto mediaPlayer = createPlayer();

    LOG(plog::debug) << "url: " << url;
    mediaPlayer->setUrl(url);
    mediaPlayer->play();

    LOG(plog::info) << "Press enter to stop playing";
    std::cin.get();
    mediaPlayer->stop();
}
