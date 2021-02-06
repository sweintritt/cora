#include "logging/file_appender.hpp"
#include "logging/logger.hpp"

FileAppender::FileAppender(const std::string& filename) {
    m_file.open(filename);
}

FileAppender::~FileAppender() {
    m_file.close();
}

void FileAppender::print(const int level, const Date& date, const std::string& name,
    const std::string& msg) {
    m_file << date.toString() << " " << Logger::NAMES[level];
    m_file << " [" << name << "] " << msg << std::endl;
}

void FileAppender::print(const int level, const Date& date, const std::string& name,
    const std::string& msg, const std::exception &e) {
    m_file << date.toString() << " " << Logger::NAMES[level];
    m_file << " [" << name << "] " << msg << ": " << e.what() << std::endl;
}

