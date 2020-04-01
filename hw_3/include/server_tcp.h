#ifndef SERVER_TCP_H_
#define SERVER_TCP_H_

#include <socket.h>
#include <connection_tcp.h>

#include <chrono>

namespace Network {

class ServerTcp : public Socket {    
    public:
        ServerTcp();
        explicit ServerTcp(const IpAddress& address);
        ServerTcp(ServerTcp&& server) = default;
        ServerTcp& operator=(ServerTcp&& server) = default;
        ~ServerTcp() = default;
       
        Socket::SockStatus listen(); 
        Socket::SockStatus accept(ConnectionTcp& connection);
    
        Socket::SockStatus restart();
        Socket::SockStatus restart(const IpAddress& address);

        std::size_t write(const void* data, std::size_t size) = delete;
        void writeExact(const void* data, std::size_t size) = delete;
        std::size_t read(void* data, std::size_t size) = delete;
        void readExact(void* data, std::size_t size) = delete;

        void setMaxConnections(uint16_t count);

    private:
        IpAddress server_addr_;
        uint16_t max_connections_;
};

}  // namespace Network

#endif  // SERVER_TCP_H_
