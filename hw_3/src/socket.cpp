#include <socket.h>

#include <socket_manager.h>

#include <utility>

#include <iostream> 

inline static bool IS_NONBLOCK_ERRNO() { 
    return (errno == EAGAIN || errno == EWOULDBLOCK);
}

namespace Network {

Socket::Socket() 
    : sock_(-1)
    , state_(Socket::DISCONNECT)
    , is_blocking_(true) {}

Socket::Socket(int socket)
    : sock_(socket)
    , state_(Socket::OK)
    , is_blocking_(true) {}

Socket::Socket(Socket&& socket)
    : state_(socket.state_)
    , is_blocking_(socket.is_blocking_) {
    std::swap(sock_, socket.sock_);    
}

Socket& Socket::operator=(Socket&& socket) {
    if (this != &socket) {
        std::swap(sock_, socket.sock_);
        state_ = socket.state_;
        is_blocking_ = socket.is_blocking_;
    }
    return *this;
}

std::size_t Socket::write(const void* data, std::size_t size) {
    if(!isOpened()) {
        return 0;
    }

    ssize_t bytes = ::send(sock_, data, size, MSG_NOSIGNAL);
    if (bytes == -1 && IS_NONBLOCK_ERRNO()) {
        return 0;
    }
    if (bytes == -1 && errno == EPIPE) {
        state_ = Socket::DISCONNECT;
        return 0;
    }
    if (bytes == -1 && errno != EPIPE) {
        throw std::runtime_error(std::strerror(errno));
    } 
    return static_cast<std::size_t>(bytes);  
}

void Socket::writeExact(const void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while(rest != size) {
        old_rest = rest;
        rest += write(static_cast<const char*>(data) + rest, size - rest);
        if((rest - old_rest) == 0) {
            throw std::runtime_error("writeExact");
        }
    }
}

std::size_t Socket::read(void* data, std::size_t size) {
    if(!isOpened()) {
        return 0;
    } 

    ssize_t bytes = ::read(sock_, data, size);
    if (bytes == -1 && !IS_NONBLOCK_ERRNO() && errno != ECONNRESET) {
        std::cout << 2 << std::endl;
        throw std::runtime_error(std::strerror(errno));
    } else if ((bytes == 0 && isBlocking()) ||
                bytes == -1 && errno == ECONNRESET) {
        state_ = Socket::DISCONNECT;
        return 0;
    } else if (bytes == -1 && IS_NONBLOCK_ERRNO()) {
        return 0;
    }
    return static_cast<std::size_t>(bytes);
}

void Socket::readExact(void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while (rest != size) {
        old_rest = rest;
        rest += read(static_cast<char*>(data) + rest, size - rest);
        if ((rest - old_rest) == 0) {
            throw std::runtime_error("");
        }
    }
}

int Socket::getSocket() const {
    return sock_;
}

void Socket::close() {
    ::close(sock_);

    state_ = Socket::DISCONNECT;
}

void Socket::setBlocking(bool to_block) {
    if (is_blocking_ ^ to_block) {
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

}
