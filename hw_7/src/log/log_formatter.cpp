#include "log_formatter.h"

#include <iomanip>
#include <ctime>

#define TIME_FORMAT "%c"

namespace Network::Log {

LogFormatter::LogFormatter(const std::string& msg, Level lvl) 
    : message_(msg)
    , level_(lvl) {}


std::ostream& operator<<(std::ostream& os, const LogFormatter& formatter) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    os << formatter.getLevel() << ":[" << std::put_time(&tm, TIME_FORMAT) << "]: "<< formatter.getMessage();
    return os;
}

std::string LogFormatter::getLevel() const {
    return LEVEL[level()];
}

std::string LogFormatter::getMessage() const {
    return message();
}

Level LogFormatter::level() const {
    return level_;
}

std::string LogFormatter::message() const {
    return message_;
}

} // namespace Network::Log
