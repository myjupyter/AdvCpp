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

#define ANSI_COLOR_GREEN   "\x1b[1;32m"
#define ANSI_COLOR_WHITE   "\x1b[1;1m"
#define ANSI_COLOR_YELLOW  "\x1b[1;33m"
#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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

