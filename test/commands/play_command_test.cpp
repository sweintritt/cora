
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
#include "testutils/string_stream_appender.hpp"

std::shared_ptr<StationsDao> stationsDao = nullptr;
std::shared_ptr<SettingsDao> settingsDao = nullptr;
std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
std::shared_ptr<Station> station = nullptr;
std::shared_ptr<PlayCommand> cmd = nullptr;

TEST_GROUP(PlayCommandTest) {
    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<PlayCommand>(stationsDao, settingsDao, mediaPlayer);
        station = std::make_shared<Station>();
        station->setAddedBy("user");
        station->setName("Sound of Movies");
        station->setGenre("Soundtracks");
        station->setLanguage("English");
        station->setCountry("England");
        station->setDescription("cool station");
        station->addUrl("http://somehost.co.uk");
        station->addUrl("http://anotherhost.co.uk");
        station->addUrl("http://andonemorehost.co.uk");
    }
};

TEST(PlayCommandTest, noIdGiven) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    CHECK_EQUAL(cmd->getName(), "play");
    std::vector<std::string> args;
    cmd->execute(args);
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    CHECK_EQUAL("No id given. See 'cora play --help' for more information.\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, idNotFound) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);
    std::vector<std::string> args{ "--file", "foo.sqlite", "24" };
    cmd->execute(args);
    CHECK_EQUAL("No station found for id:24\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, simplePlay) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);
    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station->getId()) };
    cmd->execute(args);
    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, selectUrl) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");

    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station->getId()) + ":1" };
    cmd->execute(args);
    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[1], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, selectUrlInvalidIndex) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);

    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station->getId()) + ":13" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\nOnly 3 URLs found. Index 13 is invalid. Using default index 0\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, noLastStations) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);

    std::vector<std::string> args{ "--file", "foo.sqlite", "last" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing last station\nThere is no last played station. Selecting random.\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, playLast) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);
    settingsDao->save(Settings::LAST_PLAYED, std::to_string(station->getId()));
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), std::to_string(station->getId()));

    std::vector<std::string> args{ "--file", "foo.sqlite", "last" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing last station\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, playRandom) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    stationsDao->save(*station);

    std::vector<std::string> args{ "--file", "foo.sqlite", "random" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing random station\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, getName) {
    CHECK_EQUAL("play", cmd->getName());
}

TEST(PlayCommandTest, getDescription) {
    CHECK_EQUAL("Play a station, given by id", cmd->getDescription());
}

TEST(PlayCommandTest, getUsage) {
    const std::string usage = "NAME\n"
    "  play - Play a station, given by id\n"
    "\n"
    "SYNOPSIS\n"
    "  play [OPTIONS]\n"
    "\n"
    "DESCRIPTION\n"
    "  -h, --help\n"
    "              Show help page\n"
    "\n"
    "  -f, --file <VALUE>\n"
    "              Database file. Default is /home/sweintritt/.cora.sqlite\n\n";

    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(PlayCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "play", "--help" };
    cmd->execute(args);

    const std::string usage = "NAME\n"
    "  play - Play a station, given by id\n"
    "\n"
    "SYNOPSIS\n"
    "  play [OPTIONS]\n"
    "\n"
    "DESCRIPTION\n"
    "  -h, --help\n"
    "              Show help page\n"
    "\n"
    "  -f, --file <VALUE>\n"
    "              Database file. Default is /home/sweintritt/.cora.sqlite\n\n\n";

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
