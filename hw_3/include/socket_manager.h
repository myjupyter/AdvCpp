#ifndef SOCKET_MANAGER_H_
#define SOCKET_MANAGER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>

#include <ip_address.h>

namespace Network {

class SocketManager {
    public:    
        static int makeSocket(int type, const IpAddress& addr) {
            int sock = socket(AF_INET, type, 0);
            if (sock < 0) {
                throw std::runtime_error(std::strerror(errno));
            }
            
            sockaddr_in name = addr.getSockAddr();
            if (bind(sock, reinterpret_cast<sockaddr*>(&name), sizeof(name)) < 0) {
                throw std::runtime_error(std::strerror(errno));
            }

            return sock;
        }
};

}

#endif  // SOCKET_MANAGER_H_
