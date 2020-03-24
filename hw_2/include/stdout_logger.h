#ifndef STDOUT_LOGGER_H_
#define STDOUT_LOGGER_H_

#include <base_logger.h>

namespace Log {

class StdoutLogger : public BaseLogger {
    public:
        StdoutLogger() = default;
        ~StdoutLogger() = default;

        void flush() override;

    private:
        void log(const std::string& msg, Level level) override;
};

} // namespace Log

#endif  // STDOUT_LOGGER_H_
