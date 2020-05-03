#include <iostream>
#include <iterator>
#include <sstream>

#include <http_packet.h>


using namespace Network::Http;

int main() {

    HttpPacket packet;
    packet.setVersion(1.1);
    packet.setCode(Code::OK);

    packet.addHeader({"Date",           "Sun, 10 Oct 2010 23:26:07 GMT"});
    packet.addHeader({"Server",         "Apache/2.2.8 (Ubuntu) mod_ssl/2.2.8 OpenSSL/0.9.8g"});
    packet.addHeader({"Last-Modified",  "Sun, 26 Sep 2010 22:04:35 GMT"});
    packet.addHeader({"ETag",           "\"45b6-834-49130cc1182c0\""});
    packet.addHeader({"Accept-Ranges",  "bytes"});
    packet.addHeader({"Connection",     "close"});
    packet.addHeader({"Content-Type",   "text/html"});

    packet.setBody("Hello world!");

    std::cout << packet.toResponse();

    HttpPacket new_packet(packet.toResponse());

    std::cout << new_packet.toResponse() << std::endl;

    return 0;
} 
