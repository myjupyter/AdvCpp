#include "rw_socket.h"

#include <utility>

inline static bool IS_NONBLOCK_ERRNO() { 
    return (errno == EAGAIN || errno == EWOULDBLOCK);
}

namespace Network {

RWSocket::RWSocket() 
    : Socket() {}

RWSocket::RWSocket(SockType socket_type)
    : Socket(socket_type) {}

RWSocket::RWSocket(int socket) 
    : Socket(socket) {}

std::size_t RWSocket::write(const void* data, std::size_t size) {
    if(!isOpened()) {
        return 0;
    }

    ssize_t bytes = ::send(getSocket(), data, size, MSG_NOSIGNAL);
    if (bytes == -1 && IS_NONBLOCK_ERRNO()) {
        return 0;
    }
    if (bytes == -1 && errno == EPIPE) {
        setSocketStatus(Socket::DISCONNECT);
        return 0;
    }
    if (bytes == -1 && errno != EPIPE) {
        throw std::runtime_error(std::strerror(errno));
    } 
    return static_cast<std::size_t>(bytes);  
}

void RWSocket::writeExact(const void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while(rest != size) {
        old_rest = rest;
        rest += write(static_cast<const char*>(data) + rest, size - rest);
        if((rest - old_rest) == 0) {
            throw std::runtime_error("writeExact");
        }
    }
}

std::size_t RWSocket::read(void* data, std::size_t size) {
    if(!isOpened()) {
        return 0;
    } 

    ssize_t bytes = ::read(getSocket(), data, size);
    if (bytes == -1 && !IS_NONBLOCK_ERRNO() &&
        errno != ECONNRESET) {
        throw std::runtime_error(std::strerror(errno));
    } else if ((bytes == 0 && isBlocking()) ||
                bytes == -1 && errno == ECONNRESET) {
        setSocketStatus(Socket::DISCONNECT);
        return 0;
    } else if (bytes == -1 && IS_NONBLOCK_ERRNO()) {
        return 0;
    }
    return static_cast<std::size_t>(bytes);
}

void RWSocket::readExact(void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while (rest != size) {
        old_rest = rest;
        rest += read(static_cast<char*>(data) + rest, size - rest);
        if ((rest - old_rest) == 0) {
            throw std::runtime_error("readExact");
        }
    }
}


}
