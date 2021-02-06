#include "logging/message_only_formatter.hpp"

plog::util::nstring MessageOnlyFormatter::header() {
    return plog::util::nstring();
}

plog::util::nstring MessageOnlyFormatter::format(const plog::Record& record) {
    plog::util::nostringstream stream;
    stream << record.getMessage() << std::endl;
    return stream.str();
}