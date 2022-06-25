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

TEST(UtilsTest, toStringVector) {
    char* argv[] = { "Hello", "World" };
    const std::vector<std::string> t1 = toStringVector(2, argv);
    CHECK_EQUAL("Hello", t1[0]);
    CHECK_EQUAL("World", t1[1]);

    argv[0] = {};
    const std::vector<std::string> t2 = toStringVector(0, argv);
    CHECK_TRUE(t2.empty());
}

TEST(UtilsTest, ltrim) {
    std::string value = "";
    ltrim(value);
    CHECK_EQUAL("", value);

    value = "  ";
    ltrim(value);
    CHECK_EQUAL("", value);

    value = " f ";
    ltrim(value);
    CHECK_EQUAL("f ", value);

    value = "foo";
    ltrim(value);
    CHECK_EQUAL("foo", value);
}

TEST(UtilsTest, rtrim) {
    std::string value = "";
    rtrim(value);
    CHECK_EQUAL("", value);

    value = "  ";
    rtrim(value);
    CHECK_EQUAL("", value);

    value = " f ";
    rtrim(value);
    CHECK_EQUAL(" f", value);

    value = "foo";
    rtrim(value);
    CHECK_EQUAL("foo", value);
}

TEST(UtilsTest, trim) {
    std::string value = "";
    trim(value);
    CHECK_EQUAL("", value);

    value = "  ";
    trim(value);
    CHECK_EQUAL("", value);

    value = " f ";
    trim(value);
    CHECK_EQUAL("f", value);

    value = "foo";
    trim(value);
    CHECK_EQUAL("foo", value);
}

TEST(UtilsTest, toString) {
    const std::vector<std::string> values{ "Hello", "World"};
    CHECK_EQUAL("Hello,World", toString(values));
    CHECK_EQUAL("", toString(std::vector<std::string>()));

    const std::vector<std::string> values2{ "Hello", "World", "", ""};
    CHECK_EQUAL("Hello,World,,", toString(values2));
}
