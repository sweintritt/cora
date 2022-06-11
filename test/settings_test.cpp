
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"

TEST_GROUP(SettingsTest) { };

TEST(SettingsTest, setAndGet) {
    Settings settings;
    CHECK_EQUAL(settings.get(Settings::LAST_UPDATE), "");
    settings.set(Settings::LAST_UPDATE, "2022-01-01");
    CHECK_EQUAL(settings.get(Settings::LAST_UPDATE), "2022-01-01");

    CHECK_EQUAL(settings.get("Hello"), "");
    settings.set("Hello", "World");
    CHECK_EQUAL(settings.get("Hello"), "World");
}

TEST(SettingsTest, setAndGetAsInt) {
    Settings settings;
    CHECK_EQUAL(settings.get("count"), "");
    settings.set("count", "1701");
    CHECK_EQUAL(settings.getAsInt("count"), 1701);
}


