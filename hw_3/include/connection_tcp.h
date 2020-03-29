#ifndef CONNECTION_TCP_H_
#define CONNECTION_TCP_H_

#include <socket.h>
#include <ip_address.h>
#include <socket_manager.h>

#include <chrono>

namespace Network {

class ConnectionTcp : public Socket {    
    public:
        ConnectionTcp();
        explicit ConnectionTcp(const IpAddress& address);
        explicit ConnectionTcp(const std::string& address, uint16_t port);
        ~ConnectionTcp() = default;
        
        int connect(const IpAddress& address);
        int setWriteTimeout(std::chrono::seconds time);
        int setReadTimeout(std::chrono::seconds time);
};

}  // namespace Network

#endif  // CONNECTION_TCP_H_
