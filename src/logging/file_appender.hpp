#pragma once

#include <exception>
#include <string>
#include <fstream>

#include "logging/appender.hpp"
#include "logging/date.hpp"

class FileAppender : public Appender {
   public:
      FileAppender(const std::string& filename);

      ~FileAppender();

      virtual void print(const int level, const Date& date, const std::string& name,
               const std::string& msg);

      virtual void print(const int level, const Date& date, const std::string& name,
               const std::string& msg, const std::exception& e);

   protected:
      std::ofstream m_file;
};
