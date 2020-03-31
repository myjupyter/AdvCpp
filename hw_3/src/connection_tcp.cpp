#include <connection_tcp.h>

namespace Network {

ConnectionTcp::ConnectionTcp()
    : Socket(SocketManager::makeSocket(SOCK_STREAM))
    , dest_addr_(IpAddress()) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);    
}

ConnectionTcp::ConnectionTcp(const IpAddress& addr)
    : Socket(SocketManager::makeSocket(SOCK_STREAM))
    , dest_addr_(addr) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);    
}

ConnectionTcp::ConnectionTcp(int socket, const IpAddress& addr) 
    : Socket(socket)
    , dest_addr_(addr) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
}

ConnectionTcp::~ConnectionTcp() {
    Socket::close();
}

void ConnectionTcp::close() {
    Socket::close();
}

Socket::SockStatus ConnectionTcp::connect() {
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&dest_addr_.getSockAddr()),
                     sizeof(dest_addr_.getSockAddr())) == -1 ? Socket::ERROR : Socket::OK;
}

Socket::SockStatus ConnectionTcp::connect(const IpAddress& addr) {
    dest_addr_ = addr;
    return ::connect(getSocket(), reinterpret_cast<sockaddr*>(&dest_addr_.getSockAddr()),
                     sizeof(dest_addr_.getSockAddr())) == -1 ? Socket::ERROR : Socket::OK;
}

int ConnectionTcp::setTimeout(std::chrono::seconds time, Timeout type) {
    switch(type) {
        case Timeout::READ:
            return SocketManager::setTimeout(getSocket(), SO_RCVTIMEO, time);
        case Timeout::WRITE:
            return SocketManager::setTimeout(getSocket(), SO_SNDTIMEO, time);
        default:
            return 0;
    }
}

}  // namespace Network

