#include "file_logger.h"

#include <iostream>

#include "log_formatter.h"

namespace Log {

FileLogger::FileLogger() 
    : file_(std::string(DEFAULT_NAME) + POSTFIX) {
    if (!file_) {
        throw std::runtime_error("File creating error");
    }
}

FileLogger::FileLogger(const std::string& filename) 
    : file_(filename + POSTFIX) {
    if (!file_) {
        throw std::runtime_error("File creating error");
    }
}

FileLogger::~FileLogger() {
    file_.close();
}

void FileLogger::flush() {
    file_.flush();
}

void FileLogger::log(const std::string& msg, Level lvl) {
    if (level() <= lvl) {
        file_ << LogFormatter(msg, lvl) << std::endl;
        flush();
    }
}

} // namespace Log

