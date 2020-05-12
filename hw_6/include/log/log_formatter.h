#ifndef LOG_LOG_FORMATTER_H_
#define LOG_LOG_FORMATTER_H_

#include "common.h"

#include <ostream>
#include <unordered_map>

#define DEBUG_STR   "D "
#define INFO_STR    "I "
#define WARNING_STR "W "
#define ERROR_STR   "E "

namespace Network::Log {

static std::unordered_map<Level, const std::string> LEVEL {
    {Level::Debug, DEBUG_STR},
    {Level::Info, INFO_STR},
    {Level::Warning, WARNING_STR},
    {Level::Error, ERROR_STR},
};

class LogFormatter {
    public:
        LogFormatter() = delete;
        ~LogFormatter() = default;
        
        explicit LogFormatter(const std::string& msg, Level lvl);
        
        friend std::ostream& 
        operator<<(std::ostream& os, const LogFormatter& formatter);

        virtual std::string getLevel() const;
        virtual std::string getMessage() const;

        Level level() const;
        std::string message() const;

    private:
        std::string message_;
        Level level_;
};

} // namespace Network::Log

#endif  // LOG_LOG_FORMATTER_H_

