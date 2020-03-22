#include "log_formatter.h"

#include <iomanip>
#include <ctime>

std::map<Log::Level, std::string> COLOR {
    {Log::Level::DEBUG, std::string(ANSI_COLOR_GREEN)},
    {Log::Level::INFO, std::string(ANSI_COLOR_WHITE)},
    {Log::Level::WARNING, std::string(ANSI_COLOR_YELLOW)},
    {Log::Level::ERROR, std::string(ANSI_COLOR_RED)},
};

std::map<Log::Level, std::string> LEVEL {
    {Log::Level::DEBUG, std::string(DEBUG_STR)},
    {Log::Level::INFO, std::string(INFO_STR)},
    {Log::Level::WARNING, std::string(WARNING_STR)},
    {Log::Level::ERROR, std::string(ERROR_STR)},
};


namespace Log {

LogFormatter::LogFormatter(const std::string& msg, Level lvl) 
    : message_(msg)
    , level_(lvl) {}


std::ostream& operator<<(std::ostream& os, const LogFormatter& formatter) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    os << "[" << formatter.level() << " " << std::put_time(&tm, TIME_FORMAT) << "]: "<< formatter.message();
    return os;
}

std::string LogFormatter::level() const {
    return COLOR[level_] + LEVEL[level_] + std::string(ANSI_COLOR_RESET);
}

std::string LogFormatter::message() const {
    return message_;
}

} // namespace Log
