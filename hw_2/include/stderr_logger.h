#ifndef STDERR_LOGGER_H_
#define STDERR_LOGGER_H_

#include "base_logger.h"

namespace Log {

class StderrLogger : public BaseLogger {
    public:
        StderrLogger();
        ~StderrLogger();

        virtual void flush();

    private:
        virtual void log(const std::string& msg, Level level);
};

} // namespace Log

#endif  // STDERR_LOGGER_H_
