#include "commands/play_command.hpp"

#include <plog/Log.h>

#include <iostream>

#include "utils.hpp"

PlayCommand::PlayCommand(const std::shared_ptr<StationsDao> stationsDao, 
                         const std::shared_ptr<SettingsDao> settingsDao,
                         const std::shared_ptr<MediaPlayer> mediaPlayer)
    : Command("play", "Play a station, given by id", stationsDao, settingsDao, mediaPlayer), m_waitOnPlay(true) { }

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
    std::shared_ptr<Station> station = getStation(values);

    if (station != nullptr) {
        LOG(plog::info) << "playing " << station->getName();
        std::string url = getUrl(station, values);
        play(url);
    } else {
        LOG(plog::warning) << "No station found for id:" << values[0];
    }
}

std::shared_ptr<Station> PlayCommand::getStation(const std::vector<std::string>& values) const {
    LOG(plog::debug) << "checking: " << values[0];
    m_stationsDao->open(m_cli.getValue('f', getDefaultFile()));
    m_settingsDao->open(m_cli.getValue('f', getDefaultFile()));
    std::shared_ptr<Station> station;

    if (values[0].compare("random") == 0) {
        station = m_stationsDao->getRandom();
    } else if (values[0].compare("last") == 0) {
        std::string lastPlayed = m_settingsDao->get(Settings::LAST_PLAYED);
        if (lastPlayed.empty()) {
            LOG(plog::info) << "There is no last played station. Selecting random.";
            station = m_stationsDao->getRandom();
        } else {
            const long id = std::stol(lastPlayed);
            station = m_stationsDao->findById(id);
        }
    } else {
        const long id = std::stol(values[0]);
        station = m_stationsDao->findById(id);
    }

    m_settingsDao->save(Settings::LAST_PLAYED, std::to_string(station->getId()));
    m_stationsDao->close();
    return station;
}

std::string PlayCommand::getUrl(std::shared_ptr<Station> station, const std::vector<std::string>& values) const {
    std::string url;
    if (values.size() > 1) {
        const size_t index = std::stol(values[1]);

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

    return url;
}

void PlayCommand::play(const std::string& url) const {
    LOG(plog::debug) << "url: " << url;
    m_mediaPlayer->setUrl(url);
    m_mediaPlayer->play();

    if (m_waitOnPlay) { // For easy testing this can be deactivated
        LOG(plog::info) << "Press enter to stop playing";
        std::cin.get();
    }

    m_mediaPlayer->stop();
}

void PlayCommand::waitOnPlay(const bool waitOnPlay) {
    m_waitOnPlay = waitOnPlay;
}
