#ifndef LOG_FILE_LOGGER_H_
#define LOG_FILE_LOGGER_H_

#include "base_logger.h"

#include <fstream>

namespace Network::Log {

class FileLogger : public BaseLogger {
    public:
        FileLogger();
        explicit FileLogger(const std::string& path);
        ~FileLogger() = default;

        void flush() override;

    private:
        void log(const std::string& msg, Level level) override;
        
        std::ofstream file_;
};

} // namespace Log

#endif  // LOG_FILE_LOGGER_H_
