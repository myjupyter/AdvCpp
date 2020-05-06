#include <iostream>
#include <cstring>
#include <thread>

#include "http_server.h"
#include "http_packet.h"

using namespace Network;
using namespace Network::Services;

void func(Client& client_and_data) {
    auto& [client, package] = client_and_data;

/*
    std::string buffer;
    client >> buffer;

    Http::HttpPacket request(buffer); 
    std::cout << request.toString() << std::endl << std::endl;

    Http::HttpHeader head;
    head.makeResponse("1.1", Http::Code::OK);
    head["Server"]     =  "my_server v0.1";
    head["Connection"] =  "close";

    std::string res = std::move(head.toString());
    client << res;        
*/

    std::string buffer;    

    client >> buffer;
    client << buffer;
}

int main() {
    try {
        HttpServer server;
    
        std::thread t1([&server]{server.work();});
        std::thread t2([&server]{server.work();});
//          server.work();
        t1.join();
        t2.join();

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
