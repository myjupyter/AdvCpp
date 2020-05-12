#ifndef SERVER_TCP_H_
#define SERVER_TCP_H_

#include <chrono>

#include "connection_tcp.h"

namespace Network {

class ServerTcp : public Socket {    
    public:
        ServerTcp();
        explicit ServerTcp(const IpAddress& address);

        ServerTcp(ServerTcp&& server) = default;
        ServerTcp& operator=(ServerTcp&& server) = default;
        ~ServerTcp() = default;
       
        void listen(); 
        void accept(ConnectionTcp& connection);
    
        void restart();
        void restart(const IpAddress& address);

        void setMaxConnections(uint16_t count);

        const IpAddress& getInfo() const {
            return server_addr_;
        }

    private:
        IpAddress server_addr_;
        uint16_t max_connections_;
};

}  // namespace Network

#endif  // SERVER_TCP_H_
