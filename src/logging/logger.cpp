#include "logging/logger.hpp"

#include <sstream>

std::list<Logger*> Logger::m_loggers;

std::list<Appender*> Logger::m_appenders;

const unsigned int Logger::DEBUG = 0;

const unsigned int Logger::INFO = 1;

const unsigned int Logger::WARN = 2;

const unsigned int Logger::ERROR = 3;

const unsigned int Logger::FATAL = 4;

const unsigned int Logger::OFF = 5;

const unsigned int Logger::DEFAULT = Logger::INFO;

const unsigned int Logger::ROOT = Logger::DEBUG;

const std::string Logger::NAMES[] = {"DEBUG", "INFO ", "WARN ", "ERROR", "FATAL", "OFF", "DEFAULT", "ROOT"};

Logger::Logger(const std::string name) : m_name(name), m_level(Logger::DEFAULT) { }

Logger::Logger(const std::string name, const int level) : m_name(name), m_level(level) { }

Logger::~Logger() {
   for (std::list<Appender*>::iterator it = m_appenders.begin(); it != m_appenders.end(); ++it) {
      delete* it;
   }

   for (std::list<Logger*>::iterator it = m_loggers.begin(); it != m_loggers.end(); ++it) {
      delete* it;
   }
}

void Logger::append(const unsigned int level, const Date& date, const std::string& name,
      const std::string& msg) {
   for (std::list<Appender*>::const_iterator it = m_appenders.begin(); it != m_appenders.end(); ++it) {
      (*it)->print(level, date, name, msg);
   }
}

void Logger::append(const unsigned int level, const Date& date, const std::string& name,
      const std::string& msg, const std::exception& e) {
   for (std::list<Appender* >::const_iterator it = m_appenders.begin(); it != m_appenders.end(); ++it) {
      (*it)->print(level, date, name, msg, e);
   }
}

void Logger::debug(const std::string& msg) const {
   if (debugEnabled()) {
         Logger::append(Logger::DEBUG, Date(), m_name, msg);
      }
}

void Logger::debug(const std::string& msg, const std::exception& e) const {
   if (debugEnabled()) {
      Logger::append(Logger::DEBUG, Date(), m_name, msg, e);
   }
}

bool Logger::debugEnabled() const {
   return m_level <= Logger::DEBUG;
}

void Logger::info(const std::string& msg) const {
   if (infoEnabled()) {
         Logger::append(Logger::INFO, Date(), m_name, msg);
      }
}

void Logger::info(const std::string& msg, const std::exception& e) const {
   if (infoEnabled()) {
         Logger::append(Logger::INFO, Date(), m_name, msg, e);
      }
}

bool Logger::infoEnabled() const {
   return m_level <= Logger::INFO;
}

void Logger::warn(const std::string& msg) const {
   if (warnEnabled()) {
      Logger::append(Logger::WARN, Date(), m_name, msg);
   }
}

void Logger::warn(const std::string& msg, const std::exception& e) const {
   if (warnEnabled()) {
         Date date;
         Logger::append(Logger::WARN, Date(), m_name, msg, e);
   }
}

bool Logger::warnEnabled() const {
   return m_level <= Logger::WARN;
}

void Logger::error(const std::string& msg) const {
   if (errorEnabled()) {
      Logger::append(Logger::ERROR, Date(), m_name, msg);
   }
}

void Logger::error(const std::string& msg, const std::exception& e) const {
   if (errorEnabled()) {
      Logger::append(Logger::ERROR, Date(), m_name, msg, e);
   }
}

bool Logger::errorEnabled() const {
   return m_level <= Logger::ERROR;
}

void Logger::fatal(const std::string& msg) const {
   if (fatalEnabled()) {
      Logger::append(Logger::FATAL, Date(), m_name, msg);
   }
}

void Logger::fatal(const std::string& msg, const std::exception& e) const {
   if (fatalEnabled()) {
      Logger::append(Logger::FATAL, Date(), m_name, msg, e);
   }
}

bool Logger::fatalEnabled() const {
   return m_level <= Logger::FATAL;
}

void Logger::level(const unsigned int level) {
   if (level <= Logger::OFF)    {
      m_level = level;
   } else {
      std::stringstream stream;
      stream << "value " << level << " is not a valid debug level";
      throw stream.str();
   }
}

void Logger::add(Appender* appender) {
   m_appenders.push_back(appender);
}

void Logger::remove(Appender* appender) {
   m_appenders.remove(appender);
}

std::list<Appender*> Logger::appenders() {
   return m_appenders;
}

Logger* Logger::logger(const std::string name) {
   Logger* logger = new Logger(name, Logger::ROOT);
   Logger::m_loggers.push_back(logger);
   return logger;
}

void Logger::globalLevel(const unsigned int level) {
   for (std::list<Logger*>::iterator it = m_loggers.begin(); it != m_loggers.end(); ++it) {
      (*it)->level(level);
   }
}

