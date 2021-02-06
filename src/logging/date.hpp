#pragma once

#include <ctime>
#include <string>

class Date
{
   private:
      struct tm calendar;

      time_t timestamp;

   public:
      Date();

      Date(const Date& date);

      Date(const time_t timestamp);

      Date(const struct tm calendar);

      virtual ~Date();

      virtual bool after(const Date& date) const;

      virtual bool before(const Date& date) const;

      virtual bool equals(const Date& date) const;

      virtual std::string toString() const;
};
