#include <connection_tcp.h>

namespace Network {

ConnectionTcp::ConnectionTcp()
    : Socket(SocketManager::makeSocket()) {}

ConnectionTcp::ConnectionTcp(const IpAddress& address)
    : Socket(SocketManager::makeSocket(SOCK_STREAM, address)) {}

ConnectionTcp::ConnectionTcp(const std::string& address, uint16_t port)
    : Socket(SocketManager::makeSocket(SOCK_STREAM, IpAddress(address, port))) {}

int ConnectionTcp::connect(const IpAddress& address) {
    sockaddr_in addr_in = address.getSockAddr();
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in));
}

int ConnectionTcp::setReadTimeout(std::chrono::seconds time) {
    return SocketManager::setTimeout(getSocket(), SO_RCVTIMEO, time);
}
int ConnectionTcp::setWriteTimeout(std::chrono::seconds time) {
    return SocketManager::setTimeout(getSocket(), SO_SNDTIMEO, time);
}


}  // namespace Network

