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
        ~ServerTcp() = default;
       
        Socket::SockStatus listen(); 
        Socket::SockStatus accept(ConnectionTcp& connection);

        int setTimeout(std::chrono::seconds time);

        void setMaxConnections(uint16_t count);

    private:
        IpAddress server_addr_;
        uint16_t max_connections_;
};

}  // namespace Network

#endif  // SERVER_TCP_H_
