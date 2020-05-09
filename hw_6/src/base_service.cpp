#include "base_service.h"

#include <algorithm>

static const uint32_t MAX_EVENTS = 0xff;

namespace Network::Services {

BaseService::BaseService(int flags)
    : epoll_fd_(epoll_create1(flags))
    , events_(MAX_EVENTS)
    , timeout_usec_(-1) {
    if (epoll_fd_ == -1) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                "BaseService::BaseService");
    }
}

BaseService::~BaseService() {
    ::close(epoll_fd_);
}

int BaseService::wait() {
    int n = epoll_wait(epoll_fd_, reinterpret_cast<epoll_event*>(events_.data()),
                       events_.size(), timeout_usec_);
    if (n == -1 && errno != EINTR) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "BaseService::wait");
    }
    return n;
}

void BaseService::process(int active_con, std::function<void(Event&)> func) {
    std::for_each_n(events_.begin(), active_con, func);
}

void BaseService::setMaxEvents(std::size_t count) {
    events_.resize(count);
}

void BaseService::setTimeout(int usec) {
    if (usec < -1) 
        return;
    timeout_usec_ = usec;
}

void BaseService::setTimeout(std::chrono::milliseconds usec) {
    timeout_usec_ = usec.count();
}

EventInfo* BaseService::setObserve(int socket, uint32_t mode) {
    EventInfo* ei = new EventInfo(socket);
    epoll_event event = {.events = mode, .data = {.ptr = ei}};
    
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &event)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "BaseService::setObserve");
    }
    
    return ei;
}

void BaseService::modObserve(EventInfo* socket, uint32_t mode) {
    epoll_event event = {.events = mode, .data = {.ptr = socket}};

    if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, socket->fd, &event)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "BaseService::modObserve");
    }
}

void BaseService::delObserve(EventInfo* socket) {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket->fd, nullptr)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)), 
                                "BaseService::delObserve");

    }
    delete socket;
}

}   // namespace Network::Services 
