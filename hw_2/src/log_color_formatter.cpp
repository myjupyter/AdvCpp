#include <log_color_formatter.h>

#include <iomanip>
#include <ctime>
#include <map>

#define TIME_FORMAT "%c"

#define ANSI_COLOR_GREEN   "\x1b[1;32m"
#define ANSI_COLOR_WHITE   "\x1b[1;1m"
#define ANSI_COLOR_YELLOW  "\x1b[1;33m"
#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

std::map<Log::Level, std::string> COLOR {
    {Log::Level::DEBUG, std::string(ANSI_COLOR_GREEN)},
    {Log::Level::INFO, std::string(ANSI_COLOR_WHITE)},
    {Log::Level::WARNING, std::string(ANSI_COLOR_YELLOW)},
    {Log::Level::ERROR, std::string(ANSI_COLOR_RED)},
};

namespace Log {

LogColorFormatter::LogColorFormatter(const std::string& msg, Level lvl) 
    : LogFormatter(msg, lvl) {}

std::string LogColorFormatter::getLevel() const {
    return COLOR[level()] + LEVEL[level()] + std::string(ANSI_COLOR_RESET);
}


} // namespace Log
