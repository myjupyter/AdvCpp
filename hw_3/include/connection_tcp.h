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
        explicit ConnectionTcp(const IpAddress& addr);
        ~ConnectionTcp();

        ConnectionTcp(ConnectionTcp&& connection) = default;
        ConnectionTcp& operator=(ConnectionTcp&& connection) = default;

        Socket::SockStatus connect();
        Socket::SockStatus connect(const IpAddress& addr);

        enum Timeout {
            READ,
            WRITE,
            CONNECT,
        };

        int setTimeout(std::chrono::seconds time, Timeout type);
    

    private:
        ConnectionTcp(int socket, const IpAddress& addr);
        IpAddress dest_addr_;

        friend class ServerTcp; 
};

}  // namespace Network

#endif  // CONNECTION_TCP_H_
