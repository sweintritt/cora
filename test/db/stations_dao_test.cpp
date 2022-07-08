#include "CppUTest/TestHarness.h"
#include "db/stations_dao.hpp"
#include <plog/Log.h>

TEST_GROUP(StationsDaoTest) { 

    StationsDao dao;
    Station station1;
    Station station2;
    Station station3;

    void setup() {
        dao.open(":memory:");
        station1.setAddedBy("user");
        station1.setName("Sound of Movies");
        station1.setGenre("Soundtracks");
        station1.setLanguage("English");
        station1.setCountry("England");
        station1.setDescription("cool station");
        station1.addUrl("http://somehost.co.uk");
        station1.addUrl("http://anotherhost.co.uk");
        station1.addUrl("http://andonemorehost.co.uk"); 
        dao.save(station1);

        station2.setAddedBy("foo");
        station2.setName("Sound of Rock");
        station2.setGenre("Rock");
        station2.setLanguage("German");
        station2.setCountry("Germany");
        station2.setDescription("the new station");
        station2.addUrl("http://somehost.de");

        station3.setAddedBy("user");
        station3.setName("Sound of Metal");
        station3.setGenre("Metal");
        station3.setLanguage("Finish");
        station3.setCountry("Finland");
        station3.setDescription("the new metal station");
        station3.addUrl("http://somehost.org");
    }

   void teardown() {
      dao.close();
   }
};

TEST(StationsDaoTest, findById) {
    CHECK_EQUAL(1, dao.getAllIds().size());

    const int64_t id = station1.getId();
    const std::shared_ptr<Station> actual = dao.findById(id);
    CHECK_EQUAL(station1.getAddedBy(), actual->getAddedBy());
    CHECK_EQUAL(station1.getName(), actual->getName());
    CHECK_EQUAL(station1.getGenre(), actual->getGenre());
    CHECK_EQUAL(station1.getLanguage(), actual->getLanguage());
    CHECK_EQUAL(station1.getCountry(), actual->getCountry());
    CHECK_EQUAL(station1.getDescription(), actual->getDescription());
    CHECK_EQUAL(station1.getUrls().size(), actual->getUrls().size());
    for (unsigned int i = 0; i < actual->getUrls().size(); ++i) {
        CHECK_EQUAL(station1.getUrls()[i], actual->getUrls()[i]);
    }
}

TEST(StationsDaoTest, transaction) {
    CHECK_EQUAL(1, dao.getAllIds().size());

    dao.beginTransaction();
    dao.save(station2);
    dao.rollback();
    CHECK_EQUAL(1, dao.getAllIds().size());

    dao.beginTransaction();
    dao.save(station2);
    dao.commit();
    CHECK_EQUAL(2, dao.getAllIds().size());
}

TEST(StationsDaoTest, find) {
    dao.save(station2);
    dao.save(station3);

    CHECK_EQUAL(station1.getId(), dao.find("Sound of Movies", "", "")[0]);
    CHECK_EQUAL(station2.getId(), dao.find("Sound of Rock", "", "")[0]);
    CHECK_EQUAL(station3.getId(), dao.find("Sound of Metal", "", "")[0]);
    CHECK(dao.find("Foo", "", "").empty());

    CHECK_EQUAL(station1.getId(), dao.find("", "Soundtracks", "")[0]);
    CHECK_EQUAL(station2.getId(), dao.find("", "Rock", "")[0]);
    CHECK_EQUAL(station3.getId(), dao.find("", "Metal", "")[0]);
    CHECK(dao.find("", "Foo", "").empty());

    CHECK_EQUAL(station1.getId(), dao.find("", "", "England")[0]);
    CHECK_EQUAL(station2.getId(), dao.find("", "", "Germany")[0]);
    CHECK_EQUAL(station3.getId(), dao.find("", "", "Finland")[0]);
    CHECK(dao.find("", "", "Foo").empty());

    CHECK_EQUAL(station1.getId(), dao.find("Sound of Movies", "Soundtracks", "England")[0]);
    CHECK_EQUAL(station2.getId(), dao.find("Sound of Rock", "Rock", "Germany")[0]);
    CHECK_EQUAL(station3.getId(), dao.find("Sound of Metal", "Metal", "Finland")[0]);
    CHECK(dao.find("Foo", "Foo", "Foo").empty());
    
    CHECK(dao.find("Sound of Movies", "Soundtracks", "Germany").empty());
    CHECK(dao.find("Sound of Rock", "Pop", "Germany").empty());
    CHECK(dao.find("Sound of Foo", "Metal", "Finland").empty());

    CHECK_EQUAL(station1.getId(), dao.find("", "Soundtracks", "England")[0]);
    CHECK_EQUAL(station2.getId(), dao.find("", "Rock", "Germany")[0]);
    CHECK_EQUAL(station3.getId(), dao.find("", "Metal", "Finland")[0]);
}

TEST(StationsDaoTest, deleteAllAddedBy) {
    CHECK_EQUAL(1, dao.getAllIds().size());
    dao.save(station2);
    CHECK_EQUAL(2, dao.getAllIds().size());
    CHECK(dao.findById(station2.getId()) != nullptr);
    dao.deleteAllAddedBy("foo");
    CHECK_EQUAL(1, dao.getAllIds().size());
    CHECK(dao.findById(station2.getId()) == nullptr);
}

TEST(StationsDaoTest, getRandom) {
    dao.save(station2);
    dao.save(station3);

    std::vector<int64_t> ids;
    ids.push_back(dao.getRandom()->getId());
    ids.push_back(dao.getRandom()->getId());
    ids.push_back(dao.getRandom()->getId());
    ids.push_back(dao.getRandom()->getId());
    ids.push_back(dao.getRandom()->getId());

    CHECK(ids.size() > 1);
}

TEST(StationsDaoTest, getRandomOnEmptyDatabase) {
    dao.deleteAllAddedBy("user");
    CHECK(dao.getRandom() == nullptr);
}