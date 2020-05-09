#ifndef BASE_SERVICE_H_
#define BASE_SERVICE_H_

#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <chrono>
#include <functional>
#include <system_error>
#include <tuple>

#include "non_copyable.h"
#include "coro.h"

namespace Network::Services {

struct EventInfo {
    EventInfo() = default;
    EventInfo(int fd) : fd{fd}, rout{} {}
    ~EventInfo() = default;

    int fd = -1;
    Coro::Routine rout;
};

struct Event {
        Event() {
            event_.data.ptr = nullptr;
            event_.events = 0;
        }
        
        ~Event() = default;

        int getMode() const {
            return event_.events;
        }

        EventInfo* getEventInfo() {
            return reinterpret_cast<EventInfo*>(event_.data.ptr);
        }


    public:
        epoll_event event_;
};

using Events = std::vector<Event>;

class BaseService : NonCopyable {
    public:
        BaseService(int flags = 0);
        ~BaseService(); 

        int wait();
        void process(int active_con, std::function<void(Event&)> func);

        void setMaxEvents(std::size_t count);
        void setTimeout(int usec);
        void setTimeout(std::chrono::milliseconds usec);

        EventInfo* setObserve(int socket, uint32_t mode); 
        void modObserve(EventInfo* socket, uint32_t mode);
        void delObserve(EventInfo* scoket);

    private:
        int epoll_fd_;
        mutable Events events_;
        mutable int timeout_usec_;
};

}   // namespace Network::Services 

#endif  // BASE_SERVICE_H_
