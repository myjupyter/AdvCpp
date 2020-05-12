#include "log_color_formatter.h"

#define ANSI_COLOR_GREEN   "\x1b[1;32m"
#define ANSI_COLOR_WHITE   "\x1b[1;1m"
#define ANSI_COLOR_YELLOW  "\x1b[1;33m"
#define ANSI_COLOR_RED     "\x1b[1;31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

namespace Network::Log {

static std::unordered_map<Level, const std::string> COLOR {
    {Level::Debug, ANSI_COLOR_GREEN},
    {Level::Info, ANSI_COLOR_WHITE},
    {Level::Warning, ANSI_COLOR_YELLOW},
    {Level::Error, ANSI_COLOR_RED},
};

LogColorFormatter::LogColorFormatter(const std::string& msg, Level lvl) 
    : LogFormatter(msg, lvl) {}

std::string LogColorFormatter::getLevel() const {
    return COLOR[level()] + LEVEL[level()] + std::string(ANSI_COLOR_RESET);
}


} // namespace Log
