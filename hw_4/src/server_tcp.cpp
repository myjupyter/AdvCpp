#include "server_tcp.h"

static const uint16_t MAX_CONNECTIONS = 0xffff;
static const uint16_t DEFAULT_PORT    = 8080;

namespace Network {

ServerTcp::ServerTcp()
    : Socket(SockType::TCP)
    , server_addr_(IpAddress({DEFAULT_PORT}))
    , max_connections_(MAX_CONNECTIONS) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
    SocketManager::bindSocket(getSocket(), server_addr_);        
    setSocketStatus(Socket::OK);
}

ServerTcp::ServerTcp(const IpAddress& address) 
    : Socket(SockType::TCP)
    , server_addr_(address)
    , max_connections_(MAX_CONNECTIONS) {
    SocketManager::setOption(getSocket(), SO_REUSEADDR);
    SocketManager::bindSocket(getSocket(), server_addr_);    
    setSocketStatus(Socket::OK);
}

void ServerTcp::listen() {
    try {
        SocketManager::listenSocket(getSocket(), max_connections_); 
    } catch (std::runtime_error& err) {
        throw std::runtime_error(std::string("ServerTcp::listen:") + std::string(err.what()));
    }
}

void ServerTcp::accept(ConnectionTcp& connection) {
    IpAddress client_address;
    try {
        int socket = SocketManager::accept(getSocket(), client_address);
        ConnectionTcp new_connection(socket, client_address);
        connection = std::move(new_connection); 
    } catch (std::runtime_error& err) {
        throw std::runtime_error("ServerTcp::accept:" + std::string(err.what()));
    }
}

void ServerTcp::restart() {
    restart(server_addr_);
}

void ServerTcp::restart(const IpAddress& address) {
    try {    
        ServerTcp new_server(address);    
        new_server.setMaxConnections(max_connections_);
        new_server.listen();
        *this = std::move(new_server);
    } catch (std::runtime_error& err) {
        throw std::runtime_error("ServerTcp::restart:" + std::string(err.what()));
    }    
}

void ServerTcp::setMaxConnections(uint16_t count) {
    max_connections_ = count;
}

}  // namespace Network
