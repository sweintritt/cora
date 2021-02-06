#pragma once

#include <string>

#include "loglevel.hpp"

class LogEntry {
    public:
        LogEntry(const LogLevel level, const Date& date, const std::string& name,
            const std::string& message);

        LogEntry(const LogLevel level, const Date& date, const std::string& name,
            const std::string& message, const std::exception& exception);

        virtual ~LogEntry();

        const LogLevel& level();

        const Date& date();

        const std::string& name();

        const std::string& message();

        const std::exception& exception();

    private:
        const LogLevel m_level;
        const Date m_date;
        const std::string m_name;
        const std::string m_message;
        const std::exception m_exception;
};
