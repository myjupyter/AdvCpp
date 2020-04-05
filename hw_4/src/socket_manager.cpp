#include "socket_manager.h"

namespace Network {

namespace  SocketManager {
        
int makeSocket(int type) {
    int sock = socket(AF_INET, type, 0);
    if (sock < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
    return sock;
}

int bindSocket(int socket, IpAddress& addr) { 
    if (bind(socket, reinterpret_cast<sockaddr*>(&addr.getSockAddr()),
             sizeof(addr.getSockAddr())) < 0) {
            throw std::runtime_error(std::strerror(errno));
    }
    return socket;
}

void listenSocket(int socket, int count) {
    if (listen(socket, count) < -1) {
        throw std::runtime_error(std::strerror(errno));
    }
}
    
int accept(int socket, IpAddress& addr) {
    socklen_t length = static_cast<socklen_t>(sizeof(addr.getSockAddr()));
            
    int client_socket = ::accept(socket, reinterpret_cast<sockaddr*>(&addr.getSockAddr()),
                                 &length);
    if (client_socket == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return client_socket;
}

int setTimeout(int socket, int option, std::chrono::seconds time) {
    timeval timeout{.tv_sec=time.count(), .tv_usec=0};
    return setsockopt(socket, SOL_SOCKET, option, &timeout, sizeof(timeout));
}
    
int setOption(int socket, int option) {
    int opt = 1;
    return setsockopt(socket, SOL_SOCKET, option, &opt, sizeof(opt)); 
}
        
int setBlocking(int socket) {
    int flags;
    if ((flags = fcntl(socket, F_GETFL, 0)) == -1) {
        flags = 0;
    }
    return fcntl(socket, F_SETFL, flags ^ O_NONBLOCK);
}

int getOption(int socket) {
    return fcntl(socket, F_GETFL, 0);
} 

}   // SocketManager

}   // Network
