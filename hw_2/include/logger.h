#ifndef LOGGER_H_
#define LOGGER_H_

#include <memory>

#include <base_logger.h>

namespace Log {

using GlobalLogger = std::shared_ptr<BaseLogger>;

class Logger {
    public:
        
        static Logger& getInstance();
        GlobalLogger getGlobalLogger();
        void setGlobalLogger(BaseLogger* logger);

        Logger(const Logger& logger) = delete;
        Logger& operator=(const Logger& logger) = delete;        
        
        ~Logger() = default;

    private:
        Logger() = default;
        
        GlobalLogger global_logger_;        
};

} // namespace Log

#endif  // LOGGER_H_
