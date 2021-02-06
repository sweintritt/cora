#include "logging/log_entry.hpp"

LogEntry::LogEntry(const LogLevel level, const Date& date, const std::string& name,
            const std::string& message) :
            m_level(level),
            m_date(date),
            m_name(name),
            m_message(message) { }

LogEntry::LogEntry(const LogLevel level, const Date& date, const std::string& name,
            const std::string& message, const std::exception& exception) :
            m_level(level),
            m_date(date),
            m_name(name),
            m_message(message),
            m_exception(exception) { }

LogEntry::~LogEntry() {
}

const LogLevel& LogEntry::level() {
    return m_level;
}

const Date& LogEntry::date() {
    return m_date;
}

const std::string& LogEntry::name() {
    return m_name;
}

const std::string& LogEntry::message() {
    return m_message;
}
