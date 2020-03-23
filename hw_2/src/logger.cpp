#include <logger.h>

namespace Log {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

GlobalLogger& Logger::getGlobalLogger() {
    return global_logger_;
}

void Logger::setGlobalLogger(GlobalLogger&& logger) {
    global_logger_ = std::move(logger);
}

Logger::Logger()
    : global_logger_(std::make_unique<StdoutLogger>()) {}
        
} // namespace Log

