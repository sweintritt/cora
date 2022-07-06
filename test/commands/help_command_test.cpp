
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/info_command.hpp"
#include "commands/import_command.hpp"
#include "commands/help_command.hpp"
#include "commands/list_command.hpp"
#include "commands/play_command.hpp"
#include "commands/find_command.hpp"
#include "commands/version_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(HelpCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<CommandInterpreter> commandInterpreter = nullptr;
    std::shared_ptr<HelpCommand> cmd = nullptr;
    
    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        commandInterpreter = std::make_shared<CommandInterpreter>();
        // TODO (sweintritt) get commands from Cora class, so the tests fails if a new command is added without updating the test
        commandInterpreter->add(std::unique_ptr<Command>(new InfoCommand(stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new ImportCommand(stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new HelpCommand(commandInterpreter, stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new ListCommand(stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new PlayCommand(stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new FindCommand(stationsDao, settingsDao, mediaPlayer)));
        commandInterpreter->add(std::unique_ptr<Command>(new VersionCommand(stationsDao, settingsDao, mediaPlayer)));
        cmd = std::make_shared<HelpCommand>(commandInterpreter, stationsDao, settingsDao, mediaPlayer);
    }
};

TEST(HelpCommandTest, showCommands) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "--file", "foo.sqlite" };
    cmd->execute(args);
    const std::string help = "cora - listen to internet radio stations\n"
        "   find     Find stations by name, genre and country.\n"
        "   help     Show help page\n"
        "   import   Import radio stations from different sources\n"
        "   info     Show information about a station, given by id\n"
        "   list     List all available stations\n"
        "   play     Play a station, given by id\n"
        "   version  Show the version\n";
    CHECK_EQUAL(help, STRING_STREAM_APPENDER->messages());
}

TEST(HelpCommandTest, getName) {
    CHECK_EQUAL("help", cmd->getName());
}

TEST(HelpCommandTest, getDescription) {
    CHECK_EQUAL("Show help page", cmd->getDescription());
}
