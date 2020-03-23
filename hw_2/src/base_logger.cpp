#include "base_logger.h"

namespace Log {

BaseLogger::BaseLogger() 
    : level_(Level::DEBUG) {}

BaseLogger::~BaseLogger() {}

void BaseLogger::debug(const std::string& msg) {
    lockMut();
    log(msg, Level::DEBUG);
    unlockMut();
}

void BaseLogger::info(const std::string& msg) {
    lockMut();
    log(msg, Level::INFO);
    unlockMut();   
}

        
void BaseLogger::warn(const std::string& msg) {
    lockMut();
    log(msg, Level::WARNING);
    unlockMut();
}
        
void BaseLogger::error(const std::string& msg) {
    lockMut();
    log(msg, Level::ERROR);
    unlockMut();
}

void BaseLogger::set_level(Level lvl) {
    lockMut();
    level_ = lvl;
    unlockMut();
}
        
Level BaseLogger::level() const {
    return level_;
}

void BaseLogger::flush() {}

void BaseLogger::log(const std::string& msg, Level lvl) {}

} // namespace Log

