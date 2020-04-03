#include <ip_address.h>

#include <stdexcept>
#include <arpa/inet.h>


namespace Network {

IpAddress::IpAddress() {
    addr_.sin_family      = AF_INET;
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);         
}

IpAddress::IpAddress(uint16_t port) : IpAddress() {
    setPort(port);
}

IpAddress::IpAddress(const std::string& address, uint16_t port) { 
    if (!inet_aton(address.c_str(),
                   reinterpret_cast<in_addr*>(&addr_.sin_addr))) {
        throw std::runtime_error("Bad address resolution");
    }
    addr_.sin_family      = AF_INET;
    addr_.sin_port        = htons(port);
}

IpAddress::IpAddress(uint32_t address, uint16_t port) {
    addr_.sin_family      = AF_INET;
    addr_.sin_port        = htons(port);
    addr_.sin_addr.s_addr = htonl(address);
}

void IpAddress::setPort(uint16_t port) {
    addr_.sin_port = htons(port);
}

sockaddr_in& IpAddress::getSockAddr() {
    return addr_;
}

}  // namespace Network
