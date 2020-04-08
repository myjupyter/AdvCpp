#include "base_service.h"

#include <algorithm>

static const uint32_t MAX_EVENTS = 0xff;

namespace Network::Services {

BaseService::BaseService(int flags)
    : epoll_fd_(epoll_create1(flags))
    , events_(MAX_EVENTS) {
    if (epoll_fd_ == -1) {
        throw std::runtime_error(std::string("epoll: ") + std::strerror(errno));
    }
}

BaseService::~BaseService() {
    ::close(epoll_fd_);
}

int BaseService::wait(int usec) {
    int n = epoll_wait(epoll_fd_, reinterpret_cast<epoll_event*>(events_.data()),
                       events_.size(), usec);
    if (n == -1 && errno != EINTR) {
        throw std::runtime_error(std::string("wait: ") + std::strerror(errno));
    }
    return n;
}

void BaseService::process(int active_con, std::function<void(Event&)> func) {
    std::for_each_n(events_.begin(), active_con, func);
}

void BaseService::setMaxEvents(std::size_t count) {
    events_.resize(count);
}

void BaseService::setObserve(int socket, uint32_t mode) {
    epoll_event event = {.events = mode, .data = {.fd = socket}};

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &event)) {
        throw std::runtime_error(std::string("setObserve: ") + std::strerror(errno));
    }
}

void BaseService::modObserve(int socket, uint32_t mode) {
    epoll_event event = {.events = mode, .data = {.fd = socket}};

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, socket, &event)) {
        throw std::runtime_error(std::string("modObserve: ") + std::strerror(errno));
    }
}

void BaseService::delObserve(int socket) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket, nullptr)) {
        throw std::runtime_error(std::string("delObserve: ") + std::strerror(errno));
    }
}

}   // namespace Network::Services 
