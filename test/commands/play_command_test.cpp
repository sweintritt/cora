
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

TEST_GROUP(PlayCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<Station> station1 = nullptr;
    std::shared_ptr<Station> station2 = nullptr;
    std::shared_ptr<PlayCommand> cmd = nullptr;

    void setup() override {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<PlayCommand>(stationsDao, settingsDao, mediaPlayer);

        station1 = std::make_shared<Station>();
        station1->setAddedBy("user");
        station1->setName("Sound of Movies");
        station1->setGenre("Soundtracks");
        station1->setLanguage("English");
        station1->setCountry("England");
        station1->setDescription("cool station");
        station1->addUrl("http://somehost.co.uk");
        station1->addUrl("http://anotherhost.co.uk");
        station1->addUrl("http://andonemorehost.co.uk"); 
        stationsDao->save(*station1);

        station2 = std::make_shared<Station>();
        station2->setAddedBy("user");
        station2->setName("Sound of Rock");
        station2->setGenre("Rock");
        station2->setLanguage("German");
        station2->setCountry("Germany");
        station2->setDescription("the new station");
        station2->addUrl("http://somehost.de");
        stationsDao->save(*station2);
    }
};

TEST(PlayCommandTest, noIdGiven) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    std::vector<std::string> args;
    cmd->execute(args);
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    CHECK_EQUAL("No id given. See 'cora play --help' for more information.\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, idNotFound) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    std::vector<std::string> args{ "--file", "foo.sqlite", "24" };
    cmd->execute(args);
    CHECK_EQUAL("No station found for id:24\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, simplePlay) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station1->getId()) };
    cmd->execute(args);
    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\n", STRING_STREAM_APPENDER->messages());

    STRING_STREAM_APPENDER->clear();
    args = { "--file", "foo.sqlite", std::to_string(station2->getId()) };
    cmd->execute(args);
    CHECK_EQUAL(std::to_string(station2->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station2->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Rock\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, selectUrl) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");

    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station1->getId()) + ":1" };
    cmd->execute(args);
    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[1], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, selectUrlInvalidIndex) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);

    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), "");
    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station1->getId()) + ":13" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing Sound of Movies\nOnly 3 URLs found. Index 13 is invalid. Using default index 0\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, noLastStations) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);

    std::vector<std::string> args{ "--file", "foo.sqlite", "last" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing last station\nThere is no last played station. Selecting random.\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, playLast) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);
    settingsDao->save(Settings::LAST_PLAYED, std::to_string(station1->getId()));
    CHECK_EQUAL(settingsDao->get(Settings::LAST_PLAYED), std::to_string(station1->getId()));

    std::vector<std::string> args{ "--file", "foo.sqlite", "last" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing last station\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, playRandom) {
    STRING_STREAM_APPENDER->clear();
    cmd->waitOnPlay(false);

    std::vector<std::string> args{ "--file", "foo.sqlite", "random" };
    cmd->execute(args);

    CHECK_EQUAL(std::to_string(station1->getId()), settingsDao->get(Settings::LAST_PLAYED));
    CHECK_EQUAL(station1->getUrls()[0], mediaPlayer->m_url);
    CHECK_EQUAL("playing random station\nplaying Sound of Movies\n", STRING_STREAM_APPENDER->messages());
}

TEST(PlayCommandTest, getName) {
    CHECK_EQUAL("play", cmd->getName());
}

TEST(PlayCommandTest, getDescription) {
    CHECK_EQUAL("Play a station, given by id", cmd->getDescription());
}

TEST(PlayCommandTest, getUsage) {
    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
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
        "              Database file. Default is " + filename + "\n\n";

    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(PlayCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "play", "--help" };
    cmd->execute(args);

    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
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
        "              Database file. Default is " + filename + "\n\n\n";

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
