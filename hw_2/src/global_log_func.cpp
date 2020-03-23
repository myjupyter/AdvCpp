#include <global_log_func.h>

namespace Log {

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
    Logger& logger = Logger::getInstance();
    if(logger.getGlobalLogger()) {
        logger.getGlobalLogger()->debug(msg);
    }
}

void info(const std::string& msg) {
    Logger& logger = Logger::getInstance();
    if (logger.getGlobalLogger()) {
        logger.getGlobalLogger()->info(msg);
    }
}

void warning(const std::string& msg) {
    Logger& logger = Logger::getInstance();
    if (logger.getGlobalLogger()) {
        logger.getGlobalLogger()->warn(msg);
    }
}

void error(const std::string& msg) {
    Logger& logger = Logger::getInstance();
    if (logger.getGlobalLogger()) {
        logger.getGlobalLogger()->error(msg);
    }
}

}  // namespace Log


