#ifndef CONNECTION_TCP_H_
#define CONNECTION_TCP_H_

#include "rw_socket.h"
#include "ip_address.h"
#include "socket_manager.h"

#include <chrono>

namespace Network {

class ConnectionTcp : public RWSocket {    
    public:
        ConnectionTcp();
        explicit ConnectionTcp(const IpAddress& addr);
        ~ConnectionTcp();

        ConnectionTcp(ConnectionTcp&& connection) = default;
        ConnectionTcp& operator=(ConnectionTcp&& connection) = default;

        void connect();
        void connect(const IpAddress& addr);

        // Transmit it into .cpp
        const IpAddress& getInfo() const {
            return dest_addr_;
        }

        void close();

        enum Timeout {
            READ,
            WRITE,
        };

        void setTimeout(std::chrono::seconds time, Timeout type);
    
    private:
        ConnectionTcp(int socket, const IpAddress& addr);
        IpAddress dest_addr_;

        friend class ServerTcp; 
};

}  // namespace Network

#endif  // CONNECTION_TCP_H_
