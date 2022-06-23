#include "string_stream_appender.hpp"

void StringStreamAppender::write(const plog::Record& record) {
    m_stream << record.getMessage() << std::endl;
}

std::string StringStreamAppender::messages() const {
    return m_stream.str();
}