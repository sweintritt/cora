
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>
#include <memory>

#include "CppUTest/TestHarness.h"

#include "version.hpp"
#include "commands/version_command.hpp"
#include "testutils/string_stream_appender.hpp"

TEST_GROUP(VersionCommandTest) { };

TEST(VersionCommandTest, output) {
    VersionCommand cmd(nullptr, nullptr, nullptr);
    STRING_STREAM_APPENDER->clear();
    const std::vector<std::string> args;
    cmd.execute(args);
    CHECK_EQUAL("Cora v" + CORA_VERSION + "\n", STRING_STREAM_APPENDER->messages());
}
