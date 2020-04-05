#include <iostream>

#include "base_service.h"
#include "server_tcp.h"

using namespace Network;
using namespace Network::Services;

int main() {
    try {
    BaseService service(0);
    
    ServerTcp server({"127.0.0.1", 8080});
    server.setBlocking(false);

    server.listen();

    service.setObserve(server.getSocket(), EPOLLIN);

    int n = service.wait(std::chrono::milliseconds(10000));
    service.process(n, [&server](Event& x){
        ConnectionTcp client;
        server.accept(client);

        client.write("Hello!!!",9);
        client.close();
    });
    std::cout << n << std::endl;
    

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
