#ifndef LOG_STDERR_LOGGER_H_
#define LOG_STDERR_LOGGER_H_

#include "base_logger.h"

namespace Network::Log {

class StderrLogger : public BaseLogger {
    public:
        StderrLogger() = default;
        ~StderrLogger() = default;

        void flush() override;

    private:
        void log(const std::string& msg, Level level) override;
};

} // namespace Network::Log

#endif  // LOG_STDERR_LOGGER_H_
