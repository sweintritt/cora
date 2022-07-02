
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/list_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(ListCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<Station> station = nullptr;
    std::shared_ptr<ListCommand> cmd = nullptr;

    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<ListCommand>(stationsDao, settingsDao, mediaPlayer);
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

TEST(ListCommandTest, listStations) {
    STRING_STREAM_APPENDER->clear();
    stationsDao->save(*station);
    std::vector<std::string> args{ "--file", "foo.sqlite" };
    cmd->execute(args);
    const std::string list = "id:0, name:\"Sound of Movies\", genre:\"Soundtracks\", country:\"England\"\n";
    CHECK_EQUAL(list, STRING_STREAM_APPENDER->messages());
}

TEST(ListCommandTest, getName) {
    CHECK_EQUAL("list", cmd->getName());
}

TEST(ListCommandTest, getDescription) {
    CHECK_EQUAL("List all available stations", cmd->getDescription());
}

TEST(ListCommandTest, getUsage) {
    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
        "  list - List all available stations\n\n"
        "SYNOPSIS\n"
        "  list [OPTIONS]\n\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is " + filename + "\n\n";
    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(ListCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "list", "--help" };
    cmd->execute(args);

    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
        "  list - List all available stations\n\n"
        "SYNOPSIS\n"
        "  list [OPTIONS]\n\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is " + filename + "\n\n\n";

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
