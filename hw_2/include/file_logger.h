#ifndef FILE_LOGGER_H_
#define FILE_LOGGER_H_

#include "base_logger.h"

#include <fstream>

#define DEFAULT_NAME "LOG"
#define POSTFIX ".log"

namespace Log {

class FileLogger : public BaseLogger {
    public:
        FileLogger();
        explicit FileLogger(const std::string& path);
        ~FileLogger();

        virtual void flush();

    private:
        virtual void log(const std::string& msg, Level level);
        
        std::ofstream file_;
};

} // namespace Log

#endif  // FILE_LOGGER_H_
