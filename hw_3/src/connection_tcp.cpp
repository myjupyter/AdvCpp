#include <connection_tcp.h>

namespace Network {

ConnectionTcp::ConnectionTcp()
    : Socket(SocketManager::makeSocket(SOCK_STREAM)) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);    
}

Socket::SockStatus ConnectionTcp::connect(uint32_t address, uint16_t port) {
    auto addr = IpAddress(address, port);
    sockaddr_in addr_in = addr.getSockAddr();
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in)) == -1 ? Socket::ERROR : Socket::OK;
}

Socket::SockStatus ConnectionTcp::connect(const std::string& address, uint16_t port) {
    auto addr = IpAddress(address, port);
    sockaddr_in addr_in = addr.getSockAddr();
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in)) == -1 ? Socket::ERROR : Socket::OK;
}

Socket::SockStatus ConnectionTcp::connect(const IpAddress& address) {
    sockaddr_in addr_in = address.getSockAddr();
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&addr_in), sizeof(addr_in)) == -1 ? Socket::ERROR : Socket::OK;
}

int ConnectionTcp::setReadTimeout(std::chrono::seconds time) {
    return SocketManager::setTimeout(getSocket(), SO_RCVTIMEO, time);
}
int ConnectionTcp::setWriteTimeout(std::chrono::seconds time) {
    return SocketManager::setTimeout(getSocket(), SO_SNDTIMEO, time);
}


}  // namespace Network

