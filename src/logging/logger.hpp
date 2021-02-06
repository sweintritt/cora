#pragma once

#include <list>
#include <exception>

#include "logging/appender.hpp"
#include "logging/date.hpp"

class Logger
{
   private:
      static std::list<Logger*> m_loggers;

      static std::list<Appender*> m_appenders;

      Logger(const std::string name);

      Logger(const std::string name, const int level);

      virtual ~Logger();

   protected:
      std::string m_name;

      unsigned int m_level;

      static void append(const unsigned int level, const Date& date, const std::string& name, const std::string& msg);

      static void append(const unsigned int level, const Date& date, const std::string& name, const std::string& msg, const std::exception& e);

   public:
      static const unsigned int DEBUG;

      static const unsigned int INFO;

      static const unsigned int WARN;

      static const unsigned int ERROR;

      static const unsigned int FATAL;

      static const unsigned int OFF;

      static const unsigned int DEFAULT;

      static const unsigned int ROOT;

      static const std::string NAMES[];

      virtual void debug(const std::string& msg) const;

      virtual void debug(const std::string& msg, const std::exception& e) const;

      virtual bool debugEnabled() const;

      virtual void info(const std::string& msg) const;

      virtual void info(const std::string& msg, const std::exception& e) const;

      virtual bool infoEnabled() const;

      virtual void warn(const std::string& msg) const;

      virtual void warn(const std::string& msg, const std::exception& e) const;

      virtual bool warnEnabled() const;

      virtual void error(const std::string& msg) const;

      virtual void error(const std::string& msg, const std::exception& e) const;

      virtual bool errorEnabled() const;

      virtual void fatal(const std::string& msg) const;

      virtual void fatal(const std::string& msg, const std::exception& e) const;

      virtual bool fatalEnabled() const;

      virtual void level(const unsigned int level);

      static void add(Appender* appender);

      static void remove(Appender* appender);

      static std::list<Appender*> appenders();

      static Logger* logger(const std::string name);

      static void globalLevel(const unsigned int level);
};
