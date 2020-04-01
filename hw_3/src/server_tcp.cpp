#include <server_tcp.h>

#include <iostream>

const uint16_t MAX_CONNECTIONS = 0xffff;

namespace Network {

ServerTcp::ServerTcp()
    : Socket(SocketManager::makeSocket(SOCK_STREAM))
    , server_addr_(IpAddress())
    , max_connections_(MAX_CONNECTIONS) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
    SocketManager::bindSocket(getSocket(), server_addr_);        
    setSocketStatus(Socket::OK);
}

ServerTcp::ServerTcp(const IpAddress& address) 
    : Socket(SocketManager::makeSocket(SOCK_STREAM))
    , server_addr_(address)
    , max_connections_(MAX_CONNECTIONS) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
    SocketManager::bindSocket(getSocket(), server_addr_);    
    setSocketStatus(Socket::OK);
}

Socket::SockStatus ServerTcp::listen() {
    try {
        SocketManager::listenSocket(getSocket(), max_connections_); 
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return Socket::ERROR;
    }
    return Socket::OK;
}

Socket::SockStatus ServerTcp::accept(ConnectionTcp& connection) {
    try {
        IpAddress client_address;
        int socket = SocketManager::accept(getSocket(), client_address);
        connection = std::move(ConnectionTcp(socket, client_address)); 
    } catch (std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        return Socket::ERROR;
    }
    return Socket::OK;
}

Socket::SockStatus ServerTcp::restart() {
    close();

    ServerTcp new_server(server_addr_);
    new_server.setMaxConnections(max_connections_);
    if (Socket::OK == new_server.listen()) {
        *this = std::move(new_server);
        return Socket::RECONNECT;
    }
    return Socket::ERROR;
}

Socket::SockStatus ServerTcp::restart(const IpAddress& address) {
    close();
    
    ServerTcp new_server(address);
    if (Socket::OK == new_server.listen()) {
        *this = std::move(new_server);
        return Socket::RECONNECT;
    }
    return Socket::ERROR;
}

void ServerTcp::setMaxConnections(uint16_t count) {
    max_connections_ = count;
}

}  // namespace Network
