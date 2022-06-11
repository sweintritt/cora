#pragma once

#include "commands/command.hpp"

class PlayCommand : public Command {
    public:
        PlayCommand(const std::shared_ptr<Settings> settings);
        virtual void execute(const std::vector<std::string>& args) override;

    private:
        void play(const std::string& url);
};