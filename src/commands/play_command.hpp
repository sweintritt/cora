#pragma once

#include "commands/command.hpp"

class PlayCommand : public Command {
    public:
        PlayCommand(const std::shared_ptr<StationsDao> stationsDao, 
                    const std::shared_ptr<SettingsDao> settingsDao,
                    const std::shared_ptr<MediaPlayer> mediaPlayer);
        void execute(const std::vector<std::string>& args) override;
        void waitOnPlay(const bool waitOnPlay);
    private:
        bool m_waitOnPlay;

        void play(const std::string& url) const;
        std::shared_ptr<Station> getStation(const std::vector<std::string>& values) const;
        std::string getUrl(std::shared_ptr<Station> station, const std::vector<std::string>& values) const;
};