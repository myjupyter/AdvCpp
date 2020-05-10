#include <iostream>
#include <cstring>
#include <thread>

#include "http_server.h"
#include "http_packet.h"

using namespace Network;
using namespace Network::Services;


int main() {
    try {
        HttpServer server({"127.0.0.1", 8080}, nullptr);
  
        server.work();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
