#include "file_logger.h"

#include "log_formatter.h"

namespace Network::Log {

FileLogger::FileLogger() 
    : file_("LOG.log") {
    if (!file_) {
        throw std::runtime_error("File creating error");
    }
}

FileLogger::FileLogger(const std::string& filename) 
    : file_(filename + ".log") {
    if (!file_) {
        throw std::runtime_error("File creating error");
    }
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

} // namespace Network::Log

