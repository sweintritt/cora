#include "CppUTest/TestHarness.h"
#include "sqlite_stations_dao.hpp"

TEST_GROUP(SqliteStationsDaoTest) {
};

TEST(SqliteStationsDaoTest, SaveAndGet) {
    SqliteStationsDao dao;
    dao.open("cora_test.sqlite");
    Station station;
    station.setIdHash(0);
    station.setAuthor(USER);
    station.setName("Sound of Movies");
    station.setGenre("Soundtracks");
    station.setLanguage("English");
    station.setCountry("England");
    station.setDescription("cool station");
    station.addUrl("http://somehost.co.uk");
    dao.save(station);

    CHECK_EQUAL(0, station.getIdHash());
    const long id = station.geId();

    //dao.

    dao.close();
}
