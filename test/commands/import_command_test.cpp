
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"
#include "commands/import_command.hpp"

#include "testutils/in_memory_stations_dao.hpp"
#include "testutils/in_memory_settings_dao.hpp"
#include "testutils/test_media_player.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(ImportCommandTest) {

    std::shared_ptr<StationsDao> stationsDao = nullptr;
    std::shared_ptr<SettingsDao> settingsDao = nullptr;
    std::shared_ptr<TestMediaPlayer> mediaPlayer = nullptr;
    std::shared_ptr<ImportCommand> cmd = nullptr;

    void setup() {
        stationsDao = std::make_shared<InMemoryStationsDao>();
        settingsDao = std::make_shared<InMemorySettingsDao>();
        mediaPlayer = std::make_shared<TestMediaPlayer>();
        cmd = std::make_shared<ImportCommand>(stationsDao, settingsDao, mediaPlayer);
    }
};

TEST(ImportCommandTest, unknownImporter) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "import", "--type", "foo" };
    cmd->execute(args);
    CHECK_EQUAL("Unknown importer type foo. Try 'cora import --help' for more information.\n", STRING_STREAM_APPENDER->messages());
}

TEST(ImportCommandTest, getName) {
    CHECK_EQUAL("import", cmd->getName());
}

TEST(ImportCommandTest, getDescription) {
    CHECK_EQUAL("Import radio stations from different sources", cmd->getDescription());
}

TEST(ImportCommandTest, getUsage) {
    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
        "  import - Import radio stations from different sources\n\n"
        "SYNOPSIS\n"
        "  import [OPTIONS]\n\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is " + filename + "\n\n"
        "  -i, --input <VALUE>\n"
        "              Input to import stations from. Depends on the type an could be\n"
        "              a file or URL.\n\n"
        "  -t, --type <VALUE>\n"
        "              Type of imported data. Supported types are: 'radio-sure', which\n"
        "              requires a file as input and 'radio-browser',\n"
        "              which requires a URL. 'radio-browser' is the default\n"
        "              and provides a default url.\n\n";
    CHECK_EQUAL(usage, cmd->getUsage());
}

TEST(ImportCommandTest, showHelp) {
    STRING_STREAM_APPENDER->clear();
    std::vector<std::string> args{ "info", "--help" };
    cmd->execute(args);

    const std::string filename = "/home/" + std::string(getenv("USER")) + "/.cora.sqlite";
    const std::string usage = "NAME\n"
        "  import - Import radio stations from different sources\n\n"
        "SYNOPSIS\n"
        "  import [OPTIONS]\n\n"
        "DESCRIPTION\n"
        "  -h, --help\n"
        "              Show help page\n\n"
        "  -f, --file <VALUE>\n"
        "              Database file. Default is " + filename + "\n\n"
        "  -i, --input <VALUE>\n"
        "              Input to import stations from. Depends on the type an could be\n"
        "              a file or URL.\n\n"
        "  -t, --type <VALUE>\n"
        "              Type of imported data. Supported types are: 'radio-sure', which\n"
        "              requires a file as input and 'radio-browser',\n"
        "              which requires a URL. 'radio-browser' is the default\n"
        "              and provides a default url.\n\n\n";

    CHECK_EQUAL(usage, STRING_STREAM_APPENDER->messages());
}
