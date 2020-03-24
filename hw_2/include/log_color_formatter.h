#ifndef LOG_COLOR_FORMATTER_H_
#define LOG_COLOR_FORMATTER_H_

#include <log_formatter.h>

namespace Log {

class LogColorFormatter : public LogFormatter {
    public:
        LogColorFormatter() = delete;
        ~LogColorFormatter() = default;
        
        explicit LogColorFormatter(const std::string& msg, Level lvl);
        
        std::string getLevel() const override;
};

} // namespace Log

#endif  // LOG_COLOR_FORMATTER_H_

