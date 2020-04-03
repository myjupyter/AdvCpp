#include <connection_tcp.h>

namespace Network {

ConnectionTcp::ConnectionTcp()
    : RWSocket(Socket::SockType::TCP)
    , dest_addr_(IpAddress()) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);    
}

ConnectionTcp::ConnectionTcp(const IpAddress& addr)
    : RWSocket(Socket::SockType::TCP)
    , dest_addr_(addr) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);    
}

ConnectionTcp::ConnectionTcp(int socket, const IpAddress& addr) 
    : RWSocket(socket)
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

void ConnectionTcp::connect() {
    if (isOpened()) {
        return;
    }
    int result = ::connect(getSocket(), reinterpret_cast<sockaddr*>(&dest_addr_.getSockAddr()),
                           sizeof(dest_addr_.getSockAddr()));
    if (result == -1) {
        throw std::runtime_error(std::string("connect: ") + std::strerror(errno));
    }
    setSocketStatus(Socket::OK);
}

void ConnectionTcp::connect(const IpAddress& addr) {
    if (!isOpened()) {
        dest_addr_ = addr;
        connect();
    }
    try {
        ConnectionTcp new_connection(addr);
        new_connection.connect();
        *this = std::move(new_connection);
    } catch (std::runtime_error& err) {
        throw std::runtime_error("connect: " +  std::string(err.what()));
    }
}
void ConnectionTcp::setTimeout(std::chrono::seconds time, Timeout type) {
    if (Timeout::READ == type) {
        SocketManager::setTimeout(getSocket(), SO_RCVTIMEO, time);
    } else if (Timeout::WRITE == type) {
        SocketManager::setTimeout(getSocket(), SO_SNDTIMEO, time);
    }
}

}  // namespace Network

