#include <iostream>
#include <fstream>

#include "stdout_logger.h"

int main(int argc, char* argv[]) {
    
    Log::StdoutLogger logger;

    logger.debug("Debug");
    logger.info("Info");
    logger.warn("Warn");
    logger.error("Error");

    return 0;
}
