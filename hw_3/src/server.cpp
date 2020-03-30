#include <iostream>

#include <server_tcp.h>
#include <connection_tcp.h>

#define BUFFER_SIZE 1024

int main() {

    try {    
        Network::ServerTcp server({"127.0.0.1", 8888});
        if (Network::Socket::OK == server.listen()) {

            Network::ConnectionTcp client;
            if (Network::Socket::OK == server.accept(client)) {
                std::cout << "Connection has been established" << std::endl;
            }  

            while (true) {
                std::string s;
                s.resize(BUFFER_SIZE);
            
                client.read(s.data(), s.size());
                client.write(s.data(), s.size());
            }
        }
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }   


    return 0;
}
