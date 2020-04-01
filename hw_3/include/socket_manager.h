#ifndef SOCKET_MANAGER_H_
#define SOCKET_MANAGER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>

#include <ip_address.h>

namespace Network {

class SocketManager {
    public:   
        static int makeSocket(int type = SOCK_STREAM) {
            int sock = socket(AF_INET, type, 0);
            if (sock < 0) {
                throw std::runtime_error(std::strerror(errno));
            }
            return sock;
        }

        static int bindSocket(int socket, IpAddress& addr) { 
            if (bind(socket, reinterpret_cast<sockaddr*>(&addr.getSockAddr()),
                     sizeof(addr.getSockAddr())) < 0) {
                throw std::runtime_error(std::strerror(errno));
            }

            return socket;
        }

        static void listenSocket(int socket, int count) {
            if (listen(socket, count) < -1) {
                throw std::runtime_error(std::strerror(errno));
            }
        }
    
        static int accept(int socket, IpAddress& addr) {
            socklen_t length = static_cast<socklen_t>(sizeof(addr.getSockAddr()));
            
            int client_socket = ::accept(socket,
                    reinterpret_cast<sockaddr*>(&addr.getSockAddr()),
                    &length);
            if (client_socket == -1) {
                throw std::runtime_error(std::strerror(errno));
            }
            return client_socket;
        }

        static int setTimeout(int socket, int option, std::chrono::seconds time) {
            timeval timeout{.tv_sec=time.count(), .tv_usec=0};
            return setsockopt(socket, SOL_SOCKET, option, &timeout, sizeof(timeout));
        }
    
        static int setOption(int socket, int option) {
            int opt = 1;
            return setsockopt(socket, SOL_SOCKET, option, &opt, sizeof(opt)); 
        }
        
        static int setBlocking(int socket) {
            int flags;
            if ((flags = fcntl(socket, F_GETFL, 0)) == -1)
                flags = 0;
            return fcntl(socket, F_SETFL, flags ^ O_NONBLOCK);
        }

        static int getOption(int socket) {
            return fcntl(socket, F_GETFL, 0);
        } 
};

}

#endif  // SOCKET_MANAGER_H_
