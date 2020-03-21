#ifndef STDOUT_LOGGER_H_
#define STDOUT_LOGGER_H_

#include "base_logger.h"

namespace Log {

class StdoutLogger : public BaseLogger {
    public:
        StdoutLogger();
        ~StdoutLogger();

        virtual void flush();

    private:
        virtual void log(const std::string& msg, Level level);
};

} // namespace Log

#endif  // STDOUT_LOGGER_H_
