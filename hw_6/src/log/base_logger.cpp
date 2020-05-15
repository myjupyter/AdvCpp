#include "base_logger.h"

namespace Network::Log {

BaseLogger::BaseLogger() 
    : level_(Level::Debug) {}

void BaseLogger::debug(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::Debug);
}

void BaseLogger::info(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::Info);
}
        
void BaseLogger::warn(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::Warning);
}
        
void BaseLogger::error(const std::string& msg) {
    const std::lock_guard<std::mutex> lock(mutex_);
    log(msg, Level::Error);
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

} // namespace Network::Log

