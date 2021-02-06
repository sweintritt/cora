#include "logging/console_appender.hpp"
#include "logging/logger.hpp"

#include <iostream>

ConsoleAppender::~ConsoleAppender() { }

void ConsoleAppender::print(const int level, const Date& date, const std::string& name,
    const std::string& msg)
{
    std::cout << date.toString() << " " << Logger::NAMES[level];
    std::cout << " [" << name << "] " << msg << std::endl;
}

void ConsoleAppender::print(const int level, const Date& date, const std::string& name,
    const std::string& msg, const std::exception &e)
{
    std::cout << date.toString() << " " << Logger::NAMES[level];
    std::cout << " [" << name << "] " << msg << ": " << e.what() << std::endl;
}

