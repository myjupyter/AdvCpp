#include "stdout_logger.h"

#include <iostream>

#include "log_formatter.h"

namespace Log {

StdoutLogger::StdoutLogger() {}

StdoutLogger::~StdoutLogger() {}

void StdoutLogger::flush() {
    std::cout.flush();
}

void StdoutLogger::log(const std::string& msg, Level lvl) {
    std::cout << LogFormatter(msg, lvl) << std::endl;
}

} // namespace Log

