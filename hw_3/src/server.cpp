#include <iostream>

#include <server_tcp.h>
#include <connection_tcp.h>

#define BUFFER_SIZE 1024

int main() {

    try {    
        Network::ServerTcp server({"127.0.0.1", 8888});

        server.restart({"127.0.0.1", 8889});
        if (Network::Socket::OK == server.listen()) {

            Network::ConnectionTcp client;

            while (Network::Socket::OK == server.accept(client)) {
                std::cout << "Connection has been established" << std::endl;
                while (client.isOpened()) {
                    std::string s;
                    s.resize(BUFFER_SIZE);

                    std::size_t len = client.read(s.data(), s.size());
                    client.write(s.data(), len);
                }
            }
        }
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }   


    return 0;
}
