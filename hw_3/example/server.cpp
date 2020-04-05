#include <iostream>

#include "server_tcp.h"
#include "connection_tcp.h"

const int BUFFER_SIZE = 1024;

int main() {
    try {    
        Network::ServerTcp server({"127.0.0.1", 8888});
        server.restart({"127.0.0.1", 8889});
 
        server.listen();

        Network::ConnectionTcp client;
    
        while (true) {
            server.accept(client);
            std::cout << "Connection has been established" << std::endl;
            while (client.isOpened()) {
                    std::string s(BUFFER_SIZE, '\0');

                    std::size_t len = client.read(s.data(), s.size());
                    client.writeExact(s.data(), len);
            }
            std::cout << "Connection has been closed" << std::endl;
        }
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }   

    return 0;
}
