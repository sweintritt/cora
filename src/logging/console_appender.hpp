#pragma once

#include <exception>
#include <string>

#include "logging/appender.hpp"
#include "logging/date.hpp"

class ConsoleAppender : public Appender {
   public:
      ~ConsoleAppender();

      virtual void print(const int level, const Date& date, const std::string& name,
               const std::string& msg);

      virtual void print(const int level, const Date& date, const std::string& name,
               const std::string& msg, const std::exception& e);
};
