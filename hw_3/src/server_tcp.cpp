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
}

ServerTcp::ServerTcp(const IpAddress& address) 
    : Socket(SocketManager::makeSocket(SOCK_STREAM))
    , server_addr_(address)
    , max_connections_(MAX_CONNECTIONS) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
    SocketManager::bindSocket(getSocket(), server_addr_);    
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

int ServerTcp::setTimeout(std::chrono::seconds time) {
    return 0;
}

void ServerTcp::setMaxConnections(uint16_t count) {
    max_connections_ = count;
}

}  // namespace Network
