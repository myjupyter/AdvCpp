#ifndef LOG_FORMATTER_H_
#define LOG_FORMATTER_H_

#include "base_logger.h"

#include <ostream>
#include <map>

#define TIME_FORMAT "%c"

#define DEBUG_STR "DEBUG"
#define ERROR_STR "ERROR"
#define WARNING_STR "WARNING"
#define INFO_STR "INFO"

#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/*
std::map<Log::Level, std::string> COLOR_ {
    {Log::Level::DEBUG, std::string(ANSI_COLOR_GREEN)},
    {Log::Level::INFO, std::string()},
    {Log::Level::WARNING, std::string(ANSI_COLOR_YELLOW)},
    {Log::Level::ERROR, std::string(ANSI_COLOR_RED)},
};
 
std::map<Log::Level, std::string> LEVEL_ {
    {Log::Level::DEBUG, std::string(DEBUG_STR)},
    {Log::Level::INFO, std::string(INFO_STR)},
    {Log::Level::WARNING, std::string(WARNING_STR)},
    {Log::Level::ERROR, std::string(ERROR_STR)},
};*/

namespace Log {

class LogFormatter {
    public:
        LogFormatter() = delete;
        ~LogFormatter() = default;
        
        explicit LogFormatter(const std::string& msg, Level lvl);
        
        friend std::ostream& operator<<(std::ostream& os, const LogFormatter& formatter);

    private:
        std::string level() const;
        std::string message() const;
            
        std::string message_;
        Level level_;
};

} // namespace Log

#endif  // LOG_FORMATTER_H_

