#include "CppUTest/TestHarness.h"
#include "stations_dao.hpp"
#include <plog/Log.h>

TEST_GROUP(StationsDaoTest) { };

TEST(StationsDaoTest, SaveAndGet) {
    StationsDao dao;
    dao.open(":memory:");

    CHECK_EQUAL(0, dao.getAllIds().size());

    Station expected;
    expected.setAddedBy("user");
    expected.setName("Sound of Movies");
    expected.setGenre("Soundtracks");
    expected.setLanguage("English");
    expected.setCountry("England");
    expected.setDescription("cool station");
    expected.addUrl("http://somehost.co.uk");
    expected.addUrl("http://anotherhost.co.uk");
    expected.addUrl("http://andonemorehost.co.uk");
    dao.save(expected);
    CHECK_EQUAL(1, dao.getAllIds().size());

    const int64_t id = expected.getId();
    const std::shared_ptr<Station> actual = dao.findById(id);
    CHECK_EQUAL(expected.getAddedBy(), actual->getAddedBy());
    CHECK_EQUAL(expected.getName(), actual->getName());
    CHECK_EQUAL(expected.getGenre(), actual->getGenre());
    CHECK_EQUAL(expected.getLanguage(), actual->getLanguage());
    CHECK_EQUAL(expected.getCountry(), actual->getCountry());
    CHECK_EQUAL(expected.getDescription(), actual->getDescription());
    CHECK_EQUAL(expected.getUrls().size(), actual->getUrls().size());
    for (unsigned int i = 0; i < actual->getUrls().size(); ++i) {
        CHECK_EQUAL(expected.getUrls()[i], actual->getUrls()[i]);
    }

    dao.close();
}
