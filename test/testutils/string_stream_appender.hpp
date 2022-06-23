#pragma once

#include <plog/Record.h>
#include <plog/Appenders/IAppender.h>

#include <sstream>

class StringStreamAppender : public plog::IAppender {
    public:
        void write(const plog::Record& record) override;
        std::string messages() const;
    private:
        std::ostringstream m_stream;
};