#ifndef BASE_LOGGER_H_
#define BASE_LOGGER_H_

#include <string>
#include <mutex>

#include <common.h>
//#include <thread_safety.h>

namespace Log {

class BaseLogger {
    public:
        BaseLogger();
        virtual ~BaseLogger() = default;
        
        void debug(const std::string& msg);
        void info(const std::string& msg);
        void warn(const std::string& msg);
        void error(const std::string& msg);

        void set_level(Level lvl);
        Level level() const;

        virtual void flush();

    private:
        virtual void log(const std::string& msg, Level lvl);

        Level level_;
        std::mutex mutex_;
};

} // namespace Log

#endif  // BASE_LOGGER_H_
