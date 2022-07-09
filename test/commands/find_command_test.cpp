
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/find_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(FindCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<FindCommand> cmd = nullptr;
    
    Station station1;
    Station station2;
    Station station3;

    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<FindCommand>(stationsDao, settingsDao, mediaPlayer);
        
        station1.setAddedBy("user");
        station1.setName("Sound of Movies");
        station1.setGenre("Soundtracks");
        station1.setLanguage("English");
        station1.setCountry("England");
        station1.setDescription("cool station");
        station1.addUrl("http://somehost.co.uk");
        station1.addUrl("http://anotherhost.co.uk");
        station1.addUrl("http://andonemorehost.co.uk"); 
        stationsDao->save(station1);

        station2.setAddedBy("foo");
        station2.setName("Sound of Rock");
        station2.setGenre("Rock");
        station2.setLanguage("German");
        station2.setCountry("Germany");
        station2.setDescription("the new station");
        station2.addUrl("http://somehost.de");
        stationsDao->save(station2);

        station3.setAddedBy("user");
        station3.setName("Sound of Metal");
        station3.setGenre("Metal");
        station3.setLanguage("Finish");
        station3.setCountry("Finland");
        station3.setDescription("the new metal station");
        station3.addUrl("http://somehost.org");
        stationsDao->save(station3);
    }
};

TEST(FindCommandTest, invalidSearchValues) {
    std::vector<std::vector<std::string>> values {
        std::vector<std::string>{ "find", "-n", "#+-_:.;()/&%$§" },
        std::vector<std::string>{ "find", "--name", "'; --" },
        std::vector<std::string>{ "find", "-g", "<>|" },
        std::vector<std::string>{ "find", "--genre", "\"; --" },
        std::vector<std::string>{ "find", "-c", "öäü" },
        std::vector<std::string>{ "find", "--country", "~*+?ß" }
    };

    for (const auto& args : values) {
        STRING_STREAM_APPENDER->clear();
        cmd->execute(args);
        const std::string msg = "Invalid search value: '" + args[2] 
                              + "'. See 'cora find --help' for more information.\n";
        CHECK_EQUAL(msg, STRING_STREAM_APPENDER->messages());    
    }
}

TEST(FindCommandTest, noStationsFound) {
    std::vector<std::string> args{ "find", "-n", "foo" };
    STRING_STREAM_APPENDER->clear();
    cmd->execute(args);
    CHECK_EQUAL("No stations found\n", STRING_STREAM_APPENDER->messages());    
}

TEST(FindCommandTest, stationFoundByName) {
    std::vector<std::string> args{ "find", "-n", "Sound of Movies" };
    STRING_STREAM_APPENDER->clear();
    cmd->execute(args);
    CHECK_EQUAL("id:0, name:\"Sound of Movies\", genre:\"Soundtracks\", country:\"England\"\n", STRING_STREAM_APPENDER->messages());    
}

TEST(FindCommandTest, stationFoundByGenre) {
    std::vector<std::string> args{ "find", "-g", "Rock" };
    STRING_STREAM_APPENDER->clear();
    cmd->execute(args);
    CHECK_EQUAL("id:1, name:\"Sound of Rock\", genre:\"Rock\", country:\"Germany\"\n", STRING_STREAM_APPENDER->messages());    
}

TEST(FindCommandTest, stationFoundByCountry) {
    std::vector<std::string> args{ "find", "-c", "Finland" };
    STRING_STREAM_APPENDER->clear();
    cmd->execute(args);
    CHECK_EQUAL("id:2, name:\"Sound of Metal\", genre:\"Metal\", country:\"Finland\"\n", STRING_STREAM_APPENDER->messages());    
}

TEST(FindCommandTest, getName) {
    CHECK_EQUAL("find", cmd->getName());
}

TEST(FindCommandTest, getDescription) {
    CHECK_EQUAL("Find stations by name, genre and country.", cmd->getDescription());
}

TEST(FindCommandTest, getUsage) {
    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
    "  find - Find stations by name, genre and country.\n\n"
    "SYNOPSIS\n"
    "  find [OPTIONS]\n\n"
    "DESCRIPTION\n"
    "  -h, --help\n"
    "              Show help page\n\n"
    "  -f, --file <VALUE>\n"
    "              Database file. Default is " + filename + "\n\n"
    "  -n, --name <VALUE>\n"
    "              Search by name\n\n"
    "  -g, --genre <VALUE>\n"
    "              Search by genre\n\n"
    "  -c, --country <VALUE>\n"
    "              Search by country\n\n";
    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(FindCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "find", "--help" };
    cmd->execute(args);

    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
    "  find - Find stations by name, genre and country.\n\n"
    "SYNOPSIS\n"
    "  find [OPTIONS]\n\n"
    "DESCRIPTION\n"
    "  -h, --help\n"
    "              Show help page\n\n"
    "  -f, --file <VALUE>\n"
    "              Database file. Default is " + filename + "\n\n"
    "  -n, --name <VALUE>\n"
    "              Search by name\n\n"
    "  -g, --genre <VALUE>\n"
    "              Search by genre\n\n"
    "  -c, --country <VALUE>\n"
    "              Search by country\n\n\n";

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
