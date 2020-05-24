#include "stdout_logger.h"

#include <iostream>

#include "log_color_formatter.h"

namespace Network::Log {

void StdoutLogger::flush() {
    std::cout.flush();
}

void StdoutLogger::log(const std::string& msg, Level lvl) {
    if (level() <= lvl) {
        std::cout << LogColorFormatter(msg, lvl) << std::endl;
        flush();
    }
}

} // namespace Network::Log

