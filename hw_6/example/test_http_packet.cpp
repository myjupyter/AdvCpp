#include <iostream>
#include <iterator>
#include <sstream>

#include "http_packet.h"


using namespace Network::Http;

int main() {


    HttpHeader head;
    head.makeRequest(std::string(Method::GET), "/d/d/d", "1.1");
    head["Date"] = "Sun, 10 Oct 2010 23:26:07 GMT";

    std::string buffer;
    head >> buffer;
    std::cout << buffer;

    head.makeResponse("1.1", Code::OK);
    HttpPacket packet(head.toString());
    packet.setBody("{\"key\":\"value\"}");

    packet >> buffer;
    std::cout << buffer;



    return 0;
} 
