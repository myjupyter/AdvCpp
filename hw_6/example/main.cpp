#include <iostream>
#include <cstring>
#include <thread>

#include "http_server.h"
#include "http_packet.h"
#include "global_log_func.h"

using namespace Network;
using namespace Network::Services;


int main() {
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_stderr_logger());
    try {
        HttpServer server({"127.0.0.1", 8080}, nullptr);
  
        server.work(4);

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
