#include <iostream>
#include <fstream>

#include "stdout_logger.h"
#include "stderr_logger.h"
#include "file_logger.h"

int main(int argc, char* argv[]) {
    
    std::string filename("info");

    Log::FileLogger logger(filename);
    logger.set_level(Log::Level::WARNING);

    logger.debug("Debug");
    logger.info("Info");
    logger.warn("Warn");
    logger.error("Error");

    return 0;
}
