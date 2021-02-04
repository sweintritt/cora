#pragma once

#include <vector>
#include <string>
#include <memory>

#include "stations_dao.hpp"
#include "media_player.hpp"

#include <plog/Log.h>

class Command {
    public:
        Command(const std::string& name,
                const std::shared_ptr<StationsDao> stationsDao,
                const std::shared_ptr<MediaPlayer> mediaPlayer);

        virtual ~Command();

        virtual void execute(const std::vector<std::string>& args) = 0;

        virtual const std::string& getName() const;

    protected:
        const std::string m_name;

        std::shared_ptr<StationsDao> m_stationsDao;

        std::shared_ptr<MediaPlayer> m_mediaPlayer;
};

