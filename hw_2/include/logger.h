#ifndef LOGGER_H_
#define LOGGER_H_

#include <memory>

#include <stdout_logger.h>

namespace Log {

using GlobalLogger = std::unique_ptr<BaseLogger>;

class Logger : public ThreadSafety {
    public:
        
        static Logger& getInstance();
        GlobalLogger& getGlobalLogger();
        void setGlobalLogger(GlobalLogger&& logger);

        Logger(const Logger& logger) = delete;
        Logger& operator=(const Logger& logger) = delete;        
        
        ~Logger() = default;

    private:
        Logger();
        
        GlobalLogger global_logger_;        
};

} // namespace Log

#endif  // LOGGER_H_
