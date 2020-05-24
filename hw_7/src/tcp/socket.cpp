#include "socket.h"

inline static bool IS_NONBLOCK_ERRNO() { 
    return (errno == EAGAIN || errno == EWOULDBLOCK);
}

namespace Network {

Socket::Socket(int socket) 
    : sock_(socket)
    , state_(Socket::DISCONNECT)
    , is_blocking_(true) {}

Socket::Socket() 
    : Socket(SocketManager::makeSocket(SOCK_STREAM)) {}

Socket::Socket(SockType socket_type)
    : Socket(SocketManager::makeSocket(SockType::UDP == socket_type ? SOCK_DGRAM : SOCK_STREAM)) {}

Socket::Socket(Socket&& socket)
    : is_blocking_(socket.is_blocking_) { 
    sock_ = socket.sock_;
    socket.sock_ = -1;
    
    state_ = socket.state_;    
    socket.state_ = Socket::DISCONNECT;
}

Socket& Socket::operator=(Socket&& socket) {
    if (this != &socket) {
        close();

        sock_ = socket.sock_;
        socket.sock_ = -1;

        is_blocking_ = socket.is_blocking_;
        
        state_ = socket.state_;
        socket.state_ = Socket::DISCONNECT;
    }
    return *this;
}

Socket::~Socket() {
    close();
}

int Socket::getSocket() const {
    return sock_;
}

void Socket::close() {
    if (isOpened() || sock_ != -1) {
        ::close(sock_);
        
        state_ = Socket::DISCONNECT;
        sock_  = -1; 
    }
}

void Socket::setBlocking(bool to_block) {
    if (is_blocking_ != to_block) {
        is_blocking_ = to_block;
        SocketManager::setBlocking(sock_); 
    }   
}

bool Socket::isBlocking() const {
    return is_blocking_;
}

bool Socket::isOpened() const {
    return state_ == Socket::OK;
}

Socket::SockStatus Socket::getSocketStatus() const {
    return state_;
}

void Socket::setSocketStatus(Socket::SockStatus status) {
    state_ = status;
}

}
