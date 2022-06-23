
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/play_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"

TEST_GROUP(PlayCommandTest) { };

TEST(PlayCommandTest, simplePlay) {
    auto stationsDao = std::make_shared<InMemoryStationsDao>();
    auto settingsDao = std::make_shared<InMemorySettingsDao>();
    auto mediaPlayer = std::make_shared<TestMediaPlayer>();
    PlayCommand cmd(stationsDao, settingsDao, mediaPlayer);
    cmd.waitOnPlay(false);

    Station station;
    station.setAddedBy("user");
    station.setName("Sound of Movies");
    station.setGenre("Soundtracks");
    station.setLanguage("English");
    station.setCountry("England");
    station.setDescription("cool station");
    station.addUrl("http://somehost.co.uk");
    station.addUrl("http://anotherhost.co.uk");
    station.addUrl("http://andonemorehost.co.uk");
    stationsDao->save(station);

    CHECK_EQUAL(cmd.getName(), "play");
    std::vector<std::string> args;
    cmd.execute(args);
    // TODO Check log messages

    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    args.emplace_back("--file");
    args.emplace_back("foo.sqlite");
    args.emplace_back(std::to_string(station.getId()));
    cmd.execute(args);

    CHECK_EQUAL(std::to_string(station.getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station.getUrls()[0], mediaPlayer->m_url);
}

TEST(PlayCommandTest, selectUrl) {
    auto stationsDao = std::make_shared<InMemoryStationsDao>();
    auto settingsDao = std::make_shared<InMemorySettingsDao>();
    auto mediaPlayer = std::make_shared<TestMediaPlayer>();
    PlayCommand cmd(stationsDao, settingsDao, mediaPlayer);
    cmd.waitOnPlay(false);

    Station station;
    station.setAddedBy("user");
    station.setName("Sound of Movies");
    station.setGenre("Soundtracks");
    station.setLanguage("English");
    station.setCountry("England");
    station.setDescription("cool station");
    station.addUrl("http://somehost.co.uk");
    station.addUrl("http://anotherhost.co.uk");
    station.addUrl("http://andonemorehost.co.uk");
    stationsDao->save(station);

    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    std::vector<std::string> args;
    args.emplace_back("--file");
    args.emplace_back("foo.sqlite");
    args.emplace_back(std::to_string(station.getId()) + ":1");
    cmd.execute(args);

    CHECK_EQUAL(std::to_string(station.getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station.getUrls()[1], mediaPlayer->m_url);
}
