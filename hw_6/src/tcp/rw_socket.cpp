#include "rw_socket.h"

#include <utility>

#include <iostream>

inline static bool IS_NONBLOCK_ERRNO(int error_code) { 
    return (error_code == EAGAIN || error_code == EWOULDBLOCK);
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
    if (bytes == -1 && errno == EPIPE) {
        setSocketStatus(Socket::DISCONNECT);
        return 0;
    }
    if (bytes == -1 && errno != EPIPE) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "RWSocket::write");
    } 
    return static_cast<std::size_t>(bytes);  
}

void RWSocket::writeExact(const void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while(rest != size) {
        old_rest = rest;
        try {
            rest += write(static_cast<const char*>(data) + rest, size - rest);
        } catch (std::system_error& err) {
            if (IS_NONBLOCK_ERRNO(err.code().value())) {
                continue;
            }
            setSocketStatus(Socket::DISCONNECT);
        }
        if((rest - old_rest) == 0) {
            throw std::runtime_error("RWSocket::writeExact: " + std::to_string(rest) + \
                    "/" + std::to_string(size) + " bytes were sent");
        }
    }
}

std::size_t RWSocket::read(void* data, std::size_t size) {
    if (!isOpened()) {
        return 0;
    } 

    ssize_t bytes = ::read(getSocket(), data, size);
    if (bytes == -1) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "RWSocket::read");
    }
    return static_cast<std::size_t>(bytes);
}

void RWSocket::readExact(void* data, std::size_t size) {
    std::size_t rest = 0, old_rest;

    while (rest != size) {
        old_rest = rest;
        try {
            rest += read(static_cast<char*>(data) + rest, size - rest);
        } catch(std::system_error& err) {
            if (IS_NONBLOCK_ERRNO(err.code().value())) {
                continue;
            }
            setSocketStatus(Socket::DISCONNECT);
        }
        if ((rest - old_rest) == 0) {
            setSocketStatus(Socket::DISCONNECT);
            throw std::runtime_error("RWSocket::readExact: " + std::to_string(rest) + \
                    "/" + std::to_string(size) + " bytes were recieved");
        }
    }
}

ssize_t RWSocket::read_non_block(void* data, std::size_t size) {
    if (!isOpened()) {
        return 0;
    }

    ssize_t bytes = ::read(getSocket(), data, size);
    if (bytes == -1 && !IS_NONBLOCK_ERRNO(errno)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                "RWSocket::read_non_block");
    }
    return bytes;
}


}
