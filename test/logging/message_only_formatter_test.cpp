#include "logging/message_only_formatter.hpp"

#include "CppUTest/TestHarness.h"

TEST_GROUP(MessageOnlyFormatterTest) { };

TEST(MessageOnlyFormatterTest, header) {
    MessageOnlyFormatter formatter;
    CHECK_EQUAL("", formatter.header());
}

TEST(MessageOnlyFormatterTest, format) {
    MessageOnlyFormatter formatter;
    plog::Record record(plog::error, "foo()", 13, "foo.cpp", nullptr, 21);
    record << "foo";
    CHECK_EQUAL("foo\n", formatter.format(record));
}