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
    setSocketStatus(Socket::OK);
}

ConnectionTcp::~ConnectionTcp() {
    Socket::close();
}

void ConnectionTcp::close() {
    Socket::close();
}

Socket::SockStatus ConnectionTcp::connect() {
    if (isOpened()) {
        return Socket::OK;
    }
    setSocketStatus(
            ::connect(
                getSocket(), 
                reinterpret_cast<sockaddr*>(&dest_addr_.getSockAddr()),
                sizeof(dest_addr_.getSockAddr())
            ) == -1 ? Socket::ERROR : Socket::OK
    );
    return getSocketStatus();
}

Socket::SockStatus ConnectionTcp::connect(const IpAddress& addr) {
    if (!isOpened()) {
        dest_addr_ = addr;
        return connect();
    }
    ConnectionTcp new_connection(addr);
    if (Socket::OK == new_connection.connect()) {
        *this = std::move(new_connection);
        return Socket::RECONNECT;   
    }
    return Socket::OK;
}

void ConnectionTcp::setTimeout(std::chrono::seconds time, Timeout type) {
    if (Timeout::READ == type) {
        SocketManager::setTimeout(getSocket(), SO_RCVTIMEO, time);
    } else if (Timeout::WRITE == type) {
        SocketManager::setTimeout(getSocket(), SO_SNDTIMEO, time);
    }
}

}  // namespace Network

