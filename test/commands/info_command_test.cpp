
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/info_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(InfoCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<Station> station = nullptr;
    std::shared_ptr<InfoCommand> cmd = nullptr;

    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<InfoCommand>(stationsDao, settingsDao, mediaPlayer);
        station = std::make_shared<Station>();
        station->setAddedBy("user");
        station->setName("Sound of Movies");
        station->setGenre("Soundtracks");
        station->setLanguage("English");
        station->setCountry("England");
        station->setDescription("very very very cool station. playing really really great music" 
            " and with a really really really long und totally useless description just to check"
            " the line break in the info command.");
        station->addUrl("http://somehost.co.uk");
        station->addUrl("http://anotherhost.co.uk");
        station->addUrl("http://andonemorehost.co.uk");
    }
};

TEST(InfoCommandTest, noIdGiven) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args;
    cmd->execute(args);
    CHECK_EQUAL("No id given. See 'cora info --help' for more information.\n", STRING_STREAM_APPENDER->messages());
}

TEST(InfoCommandTest, idNotFound) {
    STRING_STREAM_APPENDER->clear();
    stationsDao->save(*station);
    std::vector<std::string> args{ "--file", "foo.sqlite", "24" };
    cmd->execute(args);
    CHECK_EQUAL("No station found for id:24\n", STRING_STREAM_APPENDER->messages());
}

TEST(InfoCommandTest, showInfo) {
    STRING_STREAM_APPENDER->clear();
    stationsDao->save(*station);
    std::vector<std::string> args{ "--file", "foo.sqlite", std::to_string(station->getId()) };
    cmd->execute(args);
    const std::string info = "      station: Sound of Movies\n"
        "        genre: Soundtracks\n"
        "      country: England\n"
        "     language: English\n"
        "  description: very very very cool station. playing really really great music and with a really really\n"
        "               really long und totally useless description just to check the line break in the info\n"
        "               command.\n"
        "     added by: user\n"
        "       url[0]: http://somehost.co.uk\n"
        "       url[1]: http://anotherhost.co.uk\n"
        "       url[2]: http://andonemorehost.co.uk\n";
    CHECK_EQUAL(info, STRING_STREAM_APPENDER->messages());
}

TEST(InfoCommandTest, getName) {
    CHECK_EQUAL("info", cmd->getName());
}

TEST(InfoCommandTest, getDescription) {
    CHECK_EQUAL("Show information about a station, given by id", cmd->getDescription());
}

TEST(InfoCommandTest, getUsage) {
    const std::string usage = "NAME\n"
        "  info - Show information about a station, given by id\n"
        "\n"
        "SYNOPSIS\n"
        "  info [OPTIONS]\n"
        "\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n"
        "\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is /home/sweintritt/.cora.sqlite\n\n";
        // FIXME will not work on other machines

    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(InfoCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "play", "--help" };
    cmd->execute(args);

    const std::string usage = "NAME\n"
        "  info - Show information about a station, given by id\n"
        "\n"
        "SYNOPSIS\n"
        "  info [OPTIONS]\n"
        "\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n"
        "\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is /home/sweintritt/.cora.sqlite\n\n\n";
    // FIXME will not work on other machines

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
