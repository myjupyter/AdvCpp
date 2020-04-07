#include <iostream>
#include <cstring>

#include "base_service.h"
#include "server_tcp.h"
#include "service.h"


using namespace Network;
using namespace Network::Services;

int main() {
    try {
        Service service(IpAddress("127.0.0.1", 8080));
    
        service.setHandler([](ConnectionTcp& client){
            std::string buffer(4097, '\0');
            
            if (client.read(buffer.data(), buffer.size() - 1)) {
                std::cout << std::strlen(buffer.data()) << std::endl;
                client.writeExact(buffer.data(), std::strlen(buffer.data())); 
            }
        });
        service.work();   

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
