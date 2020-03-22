#include <stdout_logger.h>
#include <stderr_logger.h>
#include <file_logger.h>
#include <logger.h>

int main(int argc, char* argv[]) {

    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(new Log::StderrLogger());
    log.getGlobalLogger()->set_level(Log::Level::INFO);

    log.getGlobalLogger()->debug("The debug message");
    log.getGlobalLogger()->info("The info message");
    log.getGlobalLogger()->warn("The warning message");
    log.getGlobalLogger()->error("The error message");
    return 0;
}
