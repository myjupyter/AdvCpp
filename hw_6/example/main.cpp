#include <iostream>
#include <cstring>

#include "service.h"
#include "http_packet.h"

using namespace Network;
using namespace Network::Services;

void func(Client& client_and_data) {

    auto& [client, package] = client_and_data;

    client >> package;

    std::string buffer;
    while (package.getline(buffer, "\r\n\r\n")) {
        std::clog << buffer << std::endl;
    }
    std::clog << "BODY" << std::endl;
    package >> buffer;
    std::clog << buffer << std::endl;
    //client << buffer;        
}

int main() {
    try {
        Service service(IpAddress("127.0.0.1", 8080));
    
        service.setHandler(func);
        service.work();   

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
