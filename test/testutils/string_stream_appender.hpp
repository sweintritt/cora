#pragma once

#include <plog/Record.h>
#include <plog/Appenders/IAppender.h>

#include <sstream>
#include <memory>

class StringStreamAppender : public plog::IAppender {
    public:
        void write(const plog::Record& record) override;
        std::string messages() const;
        void clear();
    private:
        std::ostringstream m_stream;
};

extern const std::shared_ptr<StringStreamAppender> STRING_STREAM_APPENDER;