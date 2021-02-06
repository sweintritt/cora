#pragma once

#include <list>
#include <exception>

#include "logging/appender.hpp"
#include "logging/date.hpp"

enum LogLevel
{
    DEBUG   = 0,
    INFO    = 1,
    WARN    = 2,
    ERROR   = 3,
    FATAL   = 4,
    OFF     = 5,

    ROOT    = 0,
    DEFAULT = 2
};

const std::string LOG_LEVEL_NAMES[] = {"DEBUG", "INFO ", "WARN ", "ERROR", "FATAL", "OFF", "DEFAULT", "ROOT"};

std::string levelName(const LogLevel level)
{
    return LOG_LEVEL_NAMES[level];
}
