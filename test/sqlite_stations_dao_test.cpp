#include "CppUTest/TestHarness.h"
#include "sqlite_stations_dao.hpp"

TEST_GROUP(SqliteStationsDaoTest) {
};

TEST(SqliteStationsDaoTest, SaveAndGet) {
    SqliteStationsDao dao;
    dao.open(":memory:");
    dao.close();
}
