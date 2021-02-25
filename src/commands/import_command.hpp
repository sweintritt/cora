#pragma once

#include "commands/command.hpp"
#include "importer/importer.hpp"

#include <map>
#include <string>

class ImportCommand : public Command {
    public:
        ImportCommand(const std::shared_ptr<StationsDao> stationsDao,
                      const std::shared_ptr<MediaPlayer> mediaPlayer);

        virtual ~ImportCommand();

        virtual void execute(const std::vector<std::string>& args) override;

    private:
        std::map<std::string, std::unique_ptr<Importer>> m_importerByName;
};