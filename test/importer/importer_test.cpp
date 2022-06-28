#include "importer/radio_browser_importer.hpp"
#include "importer/radio_sure_importer.hpp"

#include "CppUTest/TestHarness.h"

TEST_GROUP(ImporterTest) { };

TEST(ImporterTest, getName) {
    RadioSureImporter radioSure;
    CHECK_EQUAL(radioSure.getName(), "radio-sure");

    RadioBrowserImporter radioBrowser;
    CHECK_EQUAL(radioBrowser.getName(), "radio-browser");
}