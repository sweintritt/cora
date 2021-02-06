#pragma once

#include <string>
#include <exception>

#include "logging/date.hpp"

class Appender {
   public:
      virtual ~Appender() { }

      virtual void print(const int level, const Date& date, const std::string& name,
            const std::string& msg) = 0;

      virtual void print(const int level, const Date& date, const std::string& name,
            const std::string& msg, const std::exception& e) = 0;
};
