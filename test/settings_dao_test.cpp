
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"

TEST_GROUP(SettingsDaoTest) { };

TEST(SettingsDaoTest, saveAndLoad) {
    SettingsDao dao;
    dao.open(":memory:");
    
    Settings settings;
    settings.set(Settings::LAST_PLAYED, "1333");
    settings.set(Settings::CORA_VERSION, "v1.0.0");
    settings.set(Settings::LAST_UPDATE, "2022-01-01");
    settings.set("Hello", "World");
    dao.save(settings);

    Settings stored = dao.load();

    CHECK_EQUAL(stored.get(Settings::LAST_PLAYED), "1333");
    CHECK_EQUAL(stored.getAsInt(Settings::LAST_PLAYED), 1333);
    CHECK_EQUAL(stored.get(Settings::CORA_VERSION), "v1.0.0");
    CHECK_EQUAL(stored.get(Settings::LAST_UPDATE), "2022-01-01");
    CHECK_EQUAL(stored.get("Hello"), "World");

    dao.close();
}
