#include <iostream>
#include <cstring>

#include "service.h"

using namespace Network;
using namespace Network::Services;

void func(Client& client_and_data) {
    ClientTcp& client = client_and_data.first;
    BytePackage& package = client_and_data.second;

    client >> package;
    
    std::string buffer;
    //  клиенту данные отправляются только тогда,
    //  когда был встречен разделитель "||"
    //  Тип условие сформированного пакета
    while (package.getline(buffer, "||")) {
        client << buffer;
    }
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
