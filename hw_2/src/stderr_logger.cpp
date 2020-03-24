#include <stderr_logger.h>

#include <iostream>

#include <log_color_formatter.h>

namespace Log {

void StderrLogger::flush() {
    std::cerr.flush();
}

void StderrLogger::log(const std::string& msg, Level lvl) {
    if (level() <= lvl) {
        std::cerr << LogColorFormatter(msg, lvl) << std::endl;
        flush();
    }
}

} // namespace Log

