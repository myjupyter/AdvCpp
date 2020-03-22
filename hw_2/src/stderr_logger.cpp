#include "stderr_logger.h"

#include <iostream>

#include "log_formatter.h"

namespace Log {

StderrLogger::StderrLogger() {}

StderrLogger::~StderrLogger() {}

void StderrLogger::flush() {
    std::cerr.flush();
}

void StderrLogger::log(const std::string& msg, Level lvl) {
    std::cerr << LogFormatter(msg, lvl) << std::endl;
}

} // namespace Log

