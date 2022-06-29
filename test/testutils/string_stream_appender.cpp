#include "string_stream_appender.hpp"

void StringStreamAppender::write(const plog::Record& record) {
    m_stream << record.getMessage() << std::endl;
}

std::string StringStreamAppender::messages() const {
    return m_stream.str();
}

void StringStreamAppender::clear() {
    m_stream.str("");
}

const std::shared_ptr<StringStreamAppender> STRING_STREAM_APPENDER = std::make_shared<StringStreamAppender>();