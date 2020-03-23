#include "base_logger.h"

namespace Log {

BaseLogger::BaseLogger() 
    : level_(Level::DEBUG) {}

BaseLogger::~BaseLogger() {}

void BaseLogger::debug(const std::string& msg) {
    log(msg, Level::DEBUG);
}

void BaseLogger::info(const std::string& msg) {
    log(msg, Level::INFO);
}
        
void BaseLogger::warn(const std::string& msg) {
    log(msg, Level::WARNING);
}
        
void BaseLogger::error(const std::string& msg) {
    log(msg, Level::ERROR);
}

void BaseLogger::set_level(Level lvl) {
    level_ = lvl;
}
        
Level BaseLogger::level() const {
    return level_;
}

void BaseLogger::flush() {}

void BaseLogger::log(const std::string& msg, Level lvl) {}

} // namespace Log

