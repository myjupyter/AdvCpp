#ifndef LOG_STDOUT_LOGGER_H_
#define LOG_STDOUT_LOGGER_H_

#include "base_logger.h"

namespace Network::Log {

class StdoutLogger : public BaseLogger {
    public:
        StdoutLogger() = default;
        ~StdoutLogger() = default;

        void flush() override;

    private:
        void log(const std::string& msg, Level level) override;
};

} // namespace Network::Log

#endif  // LOG_STDOUT_LOGGER_H_
