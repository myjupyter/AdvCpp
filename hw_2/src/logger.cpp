#include <logger.h>


namespace Log {

GlobalLogger global_logger_;

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

GlobalLogger Logger::getGlobalLogger() {
    return global_logger_;
}

void Logger::setGlobalLogger(BaseLogger* logger) {
    if (logger == nullptr) {
        return;
    }
    global_logger_.reset(logger);
}
        
} // namespace Log

