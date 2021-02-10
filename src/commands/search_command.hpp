#pragma once

#include "commands/command.hpp"

class SearchCommand : public Command {
    public:
        SearchCommand(const std::shared_ptr<StationsDao> stationsDao,
                      const std::shared_ptr<MediaPlayer> mediaPlayer);
        // TODO doesn't neet a referenze to the media player
        virtual ~SearchCommand();

        virtual void execute(const std::vector<std::string>& args) override;

        // virtual std::string getUsage()


};