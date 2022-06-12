#pragma once

#include "commands/command.hpp"

class PlayCommand : public Command {
    public:
        PlayCommand();
        void execute(const std::vector<std::string>& args) override;

    private:
        void play(const std::string& url) const;
        std::shared_ptr<Station> getStation(const std::vector<std::string>& values) const;
        std::string getUrl(std::shared_ptr<Station> station, const std::vector<std::string>& values) const;
};