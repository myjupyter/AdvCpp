#include <ip_address.h>

#include <arpa/inet.h>

constexpr uint16_t DEFAULT_PORT = 44444;

namespace Network {

IpAddress::IpAddress() {
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(DEFAULT_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);         
}

IpAddress::IpAddress(uint16_t port) : IpAddress() {
    setPort(port);
}

IpAddress::IpAddress(const std::string& address, uint16_t port) {
    if (!isValidAddr(address)) {
        throw std::runtime_error("Bad address resolution");
    }
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(address.c_str());
}

// only for ipv4
IpAddress::IpAddress(uint32_t address, uint16_t port) {
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = htonl(address);
}

void IpAddress::setPort(uint16_t port) {
    addr.sin_port = htons(port);
}

struct sockaddr_in IpAddress::getSockAddr() const {
    return addr;
}

bool IpAddress::isValidAddr(const std::string& address) const {
    struct sockaddr_in temp;
    return static_cast<bool>(inet_pton(AF_INET, address.c_str(), &temp));
}

}  // namespace Network
