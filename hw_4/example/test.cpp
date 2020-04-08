#include <iostream>
#include <cstring>
#include <thread>
#include <unistd.h>

#include "service.h"

using namespace Network;
using namespace Network::Services;

void func(Client& client_and_data) {
    ClientTcp& client = client_and_data.first;
    BytePackage& package = client_and_data.second;

    client >> package;
    client << package;
}

int main() {
    try {
        Service service(IpAddress("127.0.0.1", 8080));
                
        service.setHandler(func);
        
        std::thread working_thread([&service]() {
            try {
                service.work();
            }
            catch(std::runtime_error& err) {
                std::clog << err.what() << std::endl;
            }
        });
        
        std::thread stop_thread([&service]() {
            sleep(30);
            service.stop();
        });

        working_thread.join();
        stop_thread.join();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
