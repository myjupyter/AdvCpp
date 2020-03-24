#ifndef GLOBAL_LOG_FUNC_H_
#define GLOBAL_LOG_FUNC_H_

#include <string>
#include <memory>

#include <stdout_logger.h>
#include <stderr_logger.h>
#include <file_logger.h>
#include <logger.h>

using LogPtr = std::unique_ptr<Log::BaseLogger>;

namespace Log {
    
    LogPtr create_file_logger(const std::string& filename);
    LogPtr create_stdout_logger();
    LogPtr create_stderr_logger();

    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);

}  // namespace Log

#endif // GLOBAL_LOG_FUNC_H_
