#include <global_log_func.h>

int main(int argc, char* argv[]) {
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_file_logger("Logging"));

    Log::debug("aksldjflaksjdf");
    Log::debug("aksldjflaksjdf");
    Log::debug("aksldjflaksjdf");
    
    Log::error("aksjdf");
    Log::error("asdfalskdfj");
    return 0;
}
