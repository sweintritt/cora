#include "CppUTest/TestHarness.h"

#include "utils.hpp"

TEST_GROUP(UtilsTest) {
};

TEST(UtilsTest, split) {
    const std::vector<std::string> t1 = split("Hello World");
    CHECK_EQUAL("Hello", t1[0]);
    CHECK_EQUAL("World", t1[1]);
    CHECK_TRUE(split("").empty());

    const std::vector<std::string> t2 = split("cmd --param1 value1 --param2 value2 value3");

    CHECK_EQUAL("cmd", t2[0]);
    CHECK_EQUAL("--param1", t2[1]);
    CHECK_EQUAL("value1", t2[2]);
    CHECK_EQUAL("--param2", t2[3]);
    CHECK_EQUAL("value2", t2[4]);
    CHECK_EQUAL("value3", t2[5]);
}

TEST(UtilsTest, splitWithDelimiter) {
    const std::vector<std::string> t1 = split("Hello\tWorld", '\t');
    CHECK_EQUAL("Hello", t1[0]);
    CHECK_EQUAL("World", t1[1]);
    CHECK_TRUE(split("").empty());

    const std::vector<std::string> t2 = split("cmd;--param1;value1;--param2;value2;value3", ';');

    CHECK_EQUAL("cmd", t2[0]);
    CHECK_EQUAL("--param1", t2[1]);
    CHECK_EQUAL("value1", t2[2]);
    CHECK_EQUAL("--param2", t2[3]);
    CHECK_EQUAL("value2", t2[4]);
    CHECK_EQUAL("value3", t2[5]);
}