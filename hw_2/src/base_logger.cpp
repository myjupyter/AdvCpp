#include <base_logger.h>

namespace Log {

BaseLogger::BaseLogger() 
    : level_(Level::DEBUG) {}

void BaseLogger::debug(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::DEBUG);
}

void BaseLogger::info(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::INFO);
}
        
void BaseLogger::warn(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::WARNING);
}
        
void BaseLogger::error(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::ERROR);
}

void BaseLogger::set_level(Level lvl) {
    const std::lock_guard<std::mutex> lock(mutex_);
    level_ = lvl;
}
        
Level BaseLogger::level() const {
    return level_;
}

void BaseLogger::flush() {}

void BaseLogger::log(const std::string& msg, Level lvl) {}

} // namespace Log

