#ifndef FILE_LOGGER_H_
#define FILE_LOGGER_H_

#include <base_logger.h>

#include <fstream>

namespace Log {

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

#endif  // FILE_LOGGER_H_
