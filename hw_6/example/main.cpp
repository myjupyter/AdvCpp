#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#include "http_server.h"
#include "http_packet.h"
#include "global_log_func.h"

using namespace Network;
using namespace Network::Services;
using namespace std::chrono_literals;

int main() {
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_stderr_logger());
    try {
        HttpServer server({"127.0.0.1", 8080}, nullptr);

        std::thread sig([&server]() {
            std::this_thread::sleep_for(5s); 
            server.stop();       
        });  
        server.work(4);
        sig.join();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
