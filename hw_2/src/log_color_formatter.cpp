#include <log_color_formatter.h>

#define ANSI_COLOR_GREEN   "\x1b[1;32m"
#define ANSI_COLOR_WHITE   "\x1b[1;1m"
#define ANSI_COLOR_YELLOW  "\x1b[1;33m"
#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

static std::unordered_map<Log::Level, const std::string> COLOR {
    {Log::Level::DEBUG, ANSI_COLOR_GREEN},
    {Log::Level::INFO, ANSI_COLOR_WHITE},
    {Log::Level::WARNING, ANSI_COLOR_YELLOW},
    {Log::Level::ERROR, ANSI_COLOR_RED},
};

namespace Log {

LogColorFormatter::LogColorFormatter(const std::string& msg, Level lvl) 
    : LogFormatter(msg, lvl) {}

std::string LogColorFormatter::getLevel() const {
    return COLOR[level()] + LEVEL[level()] + std::string(ANSI_COLOR_RESET);
}


} // namespace Log
