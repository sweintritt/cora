﻿
// https://stackoverflow.com/questions/36195704/compilation-error-after-including-map
#include <map>

#include "CppUTest/TestHarness.h"

#include "db/settings.hpp"
#include "db/settings_dao.hpp"

TEST_GROUP(SettingsDaoTest) { };

TEST(SettingsDaoTest, saveAndLoad) {
    SettingsDao dao;
    dao.open(":memory:");

    CHECK_EQUAL(dao.get(Settings::LAST_PLAYED), "");
    CHECK_EQUAL(dao.get(Settings::CORA_VERSION), "");
    CHECK_EQUAL(dao.get(Settings::LAST_UPDATE), "");
    CHECK_EQUAL(dao.get("Hello"), "");

    dao.save(Settings::LAST_PLAYED, "1333");
    dao.save(Settings::CORA_VERSION, "v1.0.0");
    dao.save(Settings::LAST_UPDATE, "2022-01-01");
    dao.save("Hello", "World");

    CHECK_EQUAL(dao.get(Settings::LAST_PLAYED), "1333");
    CHECK_EQUAL(dao.get(Settings::CORA_VERSION), "v1.0.0");
    CHECK_EQUAL(dao.get(Settings::LAST_UPDATE), "2022-01-01");
    CHECK_EQUAL(dao.get("Hello"), "World");

    dao.close();
}

TEST(SettingsDaoTest, updateValues) {
    SettingsDao dao;
    dao.open(":memory:");

    CHECK_EQUAL(dao.get(Settings::LAST_PLAYED), "");
    CHECK_EQUAL(dao.get(Settings::CORA_VERSION), "");
    CHECK_EQUAL(dao.get(Settings::LAST_UPDATE), "");

    dao.save(Settings::LAST_PLAYED, "1333");
    dao.save(Settings::CORA_VERSION, "v1.0.0");
    dao.save(Settings::LAST_UPDATE, "2022-01-01");

    CHECK_EQUAL(dao.get(Settings::LAST_PLAYED), "1333");
    CHECK_EQUAL(dao.get(Settings::CORA_VERSION), "v1.0.0");
    CHECK_EQUAL(dao.get(Settings::LAST_UPDATE), "2022-01-01");

    dao.save(Settings::LAST_PLAYED, "15678");
    dao.save(Settings::CORA_VERSION, "v1.1.0");
    dao.save(Settings::LAST_UPDATE, "2022-07-01");

    CHECK_EQUAL(dao.get(Settings::LAST_PLAYED), "15678");
    CHECK_EQUAL(dao.get(Settings::CORA_VERSION), "v1.1.0");
    CHECK_EQUAL(dao.get(Settings::LAST_UPDATE), "2022-07-01");


    dao.close();
}
