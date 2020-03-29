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
        ~ConnectionTcp() = default;
        
        Socket::SockStatus connect(const IpAddress& address);
        Socket::SockStatus connect(const std::string& address, uint16_t port);
        Socket::SockStatus connect(uint32_t address, uint16_t port);        

        int setWriteTimeout(std::chrono::seconds time);
        int setReadTimeout(std::chrono::seconds time);
};

}  // namespace Network

#endif  // CONNECTION_TCP_H_
