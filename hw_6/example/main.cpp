#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>

#include "http_server.h"
#include "http_packet.h"
#include "global_log_func.h"
#include "configurator.h"

using namespace Network;
using namespace Network::Services;
using namespace std::chrono_literals;

int main() {
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_stderr_logger());

    Config::Configurator& config = Config::Configurator::getInstance();
    config.readConfigsFrom("./config/config.ini");


    std::cout << Config::Configurator::get<int>("MyHttp", "events") << std::endl;

    try {
        HttpServer server({"127.0.0.1", 8080});

        server.work(4, 2.5);

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
