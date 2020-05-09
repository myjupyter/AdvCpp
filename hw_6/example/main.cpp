#include <iostream>
#include <cstring>
#include <thread>

#include "http_server.h"
#include "http_packet.h"

using namespace Network;
using namespace Network::Services;


int main() {
    try {
        HttpServer server;
  
        std::thread t2([&server]{server.work();});
        t2.join();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
