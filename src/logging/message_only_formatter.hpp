#pragma once

#include <plog/Log.h>
#include <plog/Util.h>

class MessageOnlyFormatter {
    public:
        static plog::util::nstring header();

        static plog::util::nstring format(const plog::Record& record);
};