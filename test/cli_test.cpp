#include "CppUTest/TestHarness.h"

#include "cli/cli.hpp"

TEST_GROUP(CliTest) {
};

TEST(CliTest, simpleTest) {
    Cli cli("cmd", "test");
    cli.addOption('d', false, "debug");
    char *args[2] = {"cmd", "-d"};
    cli.parse(2, args);
    CHECK_FALSE(cli.hasOption('c'));
    CHECK_FALSE(cli.hasValue('c'));
    CHECK_TRUE(cli.hasOption('d'));
}

TEST(CliTest, complexTest) {
    Cli cli("cmd", "test");
    cli.addOption('d', "debug", false, "debug");
    cli.addOption('f', "file", true, "filename");
    cli.addOption('b', "blub", false, "blub");
    char *args[5] = {"cmd", "-d", "--file", "foo", "--blub"};
    cli.parse(5, args);
    CHECK_FALSE(cli.hasOption('c'));
    CHECK_FALSE(cli.hasValue('c'));

    CHECK_TRUE(cli.hasOption('d'));
    CHECK_TRUE(cli.hasOption("debug"));
    CHECK_FALSE(cli.hasValue('d'));
    CHECK_FALSE(cli.hasValue("debug"));

    CHECK_TRUE(cli.hasOption('f'));
    CHECK_TRUE(cli.hasOption("file"));
    CHECK_TRUE(cli.hasValue('f'));
    CHECK_TRUE(cli.hasValue("file"));
    CHECK_EQUAL("foo", cli.getValue('f'));
    CHECK_EQUAL("foo", cli.getValue("file"));

    CHECK_TRUE(cli.hasOption('b'));
    CHECK_TRUE(cli.hasOption("blub"));
    CHECK_FALSE(cli.hasValue('b'));
    CHECK_FALSE(cli.hasValue("blub"));

}