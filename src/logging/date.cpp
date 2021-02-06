#include "logging/date.hpp"

Date::Date() {
   time_t timestamp = time(0);
   calendar = *localtime(&timestamp);
}

Date::Date(const Date& date) : calendar(date.calendar), timestamp(date.timestamp) { }

Date::Date(const time_t timestamp) : calendar(*localtime(&timestamp)), timestamp(timestamp) { }

Date::Date(struct tm calendar) : calendar(calendar), timestamp(mktime(&calendar)) { }

Date::~Date() { }

bool Date::after(const Date& date) const {
   const double seconds = difftime(date.timestamp, timestamp);
   return seconds < 0;
}

bool Date::before(const Date& date) const {
   const double seconds = difftime(date.timestamp, timestamp);
   return seconds > 0;
}

bool Date::equals(const Date& date) const {
   const double seconds = difftime(date.timestamp, timestamp);
   return seconds == 0;
}

std::string Date::toString() const {
   std::string stringTime(asctime(&calendar));
   stringTime.erase(stringTime.length() - 1, 1);
   return stringTime;
}


