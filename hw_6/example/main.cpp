#include <iostream>
#include <cstring>

#include "service.h"
#include "http_packet.h"

using namespace Network;
using namespace Network::Services;

void func(Client& client_and_data) {

    auto& [client, package] = client_and_data;

    std::string buffer;
    client >> buffer;

    std::cout << buffer << std::endl;    
    Http::HttpPacket request(buffer); 

    std::cout << request.toRequest() << std::endl << std::endl;

    Http::HttpPacket packet;
    packet.setVersion(1.1);
    packet.setCode(Http::Code::OK);
    packet.addHeader({"Server", "my_server v0.1"});
    packet.addHeader({"Connection", "close"});

    std::string f = packet.toResponse();
    client << f;        
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
