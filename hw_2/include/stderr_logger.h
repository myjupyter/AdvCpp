#ifndef STDERR_LOGGER_H_
#define STDERR_LOGGER_H_

#include <base_logger.h>

namespace Log {

class StderrLogger : public BaseLogger {
    public:
        StderrLogger() = default;
        ~StderrLogger() = default;

        void flush() override;

    private:
        void log(const std::string& msg, Level level) override;
};

} // namespace Log

#endif  // STDERR_LOGGER_H_
