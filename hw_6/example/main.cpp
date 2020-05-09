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
  
        std::thread t3([&server]{server.work();});
        std::thread t2([&server]{server.work();});
        std::thread t1([&server]{server.work();});
        t3.join();
        t2.join();
        t1.join();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
