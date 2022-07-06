
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/command_interpreter.hpp"
#include "commands/help_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(CommandInterpreterTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<CommandInterpreter> commandInterpreter = nullptr;
    
    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        commandInterpreter = std::make_shared<CommandInterpreter>();
        // TODO (sweintritt) get commands from Cora class, so the tests fails if a new command is added without updating the test
        commandInterpreter->add(std::unique_ptr<Command>(new HelpCommand(commandInterpreter, stationsDao, settingsDao, mediaPlayer)));
    }
};

TEST(CommandInterpreterTest, showCommands) {
    STRING_STREAM_APPENDER->clear();
    commandInterpreter->showCommands();
    const std::string commands = "   help  Show help page\n";
    CHECK_EQUAL(commands, STRING_STREAM_APPENDER->messages());
}

TEST(CommandInterpreterTest, hasCommand) {
    CHECK_EQUAL(commandInterpreter->hasCommand("help"), true);
    CHECK_EQUAL(commandInterpreter->hasCommand("foo"), false);
}

TEST(CommandInterpreterTest, executeExistingCommand) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "cora", "help" };
    commandInterpreter->execute(args);
    const std::string help = "cora - listen to internet radio stations\n"
        "   help  Show help page\n";
    CHECK_EQUAL(help, STRING_STREAM_APPENDER->messages());
}

TEST(CommandInterpreterTest, executeNotExistingCommand) {
    std::vector<std::string> args{ "cora", "foo" };
    try {
        commandInterpreter->execute(args);
        FAIL("Exception should have been thrown at unknown command");
    } catch (const std::invalid_argument i) {
        CHECK_EQUAL("Unknown command: 'foo'. Try 'cora help' for more information.", std::string{ i.what() });
    }
}

TEST(CommandInterpreterTest, addDuplicateCommand) {
    try {
        commandInterpreter->add(std::unique_ptr<Command>(new HelpCommand(commandInterpreter, stationsDao, settingsDao, mediaPlayer)));
        FAIL("Exception should have been thrown at duplicate command");
    } catch (const std::runtime_error r) {
        CHECK_EQUAL("duplicate command: 'help'", std::string{ r.what() });
    }
}
