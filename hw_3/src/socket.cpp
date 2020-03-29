#include <socket.h>

namespace Network {

Socket::Socket() 
    : sock_(-1)
    , state_(false) {}

Socket::Socket(int socket)
    : sock_(socket)
    , state_(true) {}

Socket::~Socket() {
    if(sock_ < 0) {
        return;
    }
    ::close(sock_);
}

std::size_t Socket::write(const void* data, std::size_t size) {
    ssize_t bytes = ::write(sock_, data, size);
    if (bytes == -1) {
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
            throw std::runtime_error("");
        }
    }
}

std::size_t Socket::read(void* data, std::size_t size) {
    ssize_t bytes = ::read(sock_, data, size);

    if (bytes == -1) {
        throw std::runtime_error(std::strerror(errno));
    } else if (bytes == 0) {
        state_ = false;
    }
    return bytes;
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

bool Socket::isOpened() const {
    return state_;
}

}
