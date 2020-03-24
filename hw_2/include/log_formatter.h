#ifndef LOG_FORMATTER_H_
#define LOG_FORMATTER_H_

#include <common.h>

#include <ostream>
#include <unordered_map>

#define DEBUG_STR   "  DEBUG"
#define INFO_STR    "   INFO"
#define WARNING_STR "WARNING"
#define ERROR_STR   "  ERROR"

static std::unordered_map<Log::Level, const std::string> LEVEL {
    {Log::Level::DEBUG, DEBUG_STR},
    {Log::Level::INFO, INFO_STR},
    {Log::Level::WARNING, WARNING_STR},
    {Log::Level::ERROR, ERROR_STR},
};


namespace Log {

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

} // namespace Log

#endif  // LOG_FORMATTER_H_

