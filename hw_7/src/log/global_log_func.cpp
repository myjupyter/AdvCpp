#include "global_log_func.h"

namespace Network::Log {

LogPtr create_file_logger(const std::string& filename) {
    return std::make_unique<FileLogger>(filename);
}

LogPtr create_stdout_logger() {
    return std::make_unique<StdoutLogger>();
}

LogPtr create_stderr_logger() {
    return std::make_unique<StderrLogger>();
}

void debug(const std::string& msg) {
    auto& logger = Logger::getInstance().getGlobalLogger();
    if(logger) {
        logger->debug(msg);
    }
}

void info(const std::string& msg) {
    auto& logger = Logger::getInstance().getGlobalLogger();
    if (logger) {
        logger->info(msg);
    }
}

void warning(const std::string& msg) {
    auto& logger = Logger::getInstance().getGlobalLogger();
    if (logger) {
        logger->warn(msg);
    }
}

void error(const std::string& msg) {
    auto& logger = Logger::getInstance().getGlobalLogger();
    if (logger) {
        logger->error(msg);
    }
}

void set_level(Level level) {
    auto& logger = Logger::getInstance().getGlobalLogger();
    if (logger) {
        logger->set_level(level);
    }
}

}  // namespace Log


