#ifndef BASE_SERVICE_H_
#define BASE_SERVICE_H_

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <chrono>
#include <functional>

#include "non_copyable.h"

namespace Network::Services {

struct Event {
        Event() {
            event_.data.fd = -1;
            event_.events = 0;
        }
        Event(int socket, uint32_t mode) {
            event_.data.fd = socket;
            event_.events = mode;
        }
        ~Event() = default;

        int getMode() {
            return event_.events;
        }

        int getFd() {
            return event_.data.fd;
        }

    public:
        epoll_event event_;
};

using Events = std::vector<Event>;

class BaseService : NonCopyable {
    public:
        BaseService(int flags = 0);
        ~BaseService(); 

        int wait(int usec);
        void process(int active_con, std::function<void(Event&)> func);

        void setMaxEvents(std::size_t count);

        void setObserve(int socket, uint32_t mode); 
        void modObserve(int socket, uint32_t mode);
        void delObserve(int scoket);

    private:
        mutable Events events_;
        int epoll_fd_;
};

}   // namespace Network::Services 

#endif  // BASE_SERVICE_H_
