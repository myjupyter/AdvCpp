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
#include "client_tcp.h"

namespace Network::Services {

using Client = std::pair<ClientTcp, BytePackage>;

struct EventInfo {
    public:
        using timer = std::chrono::time_point<std::chrono::system_clock>;

    public:
        EventInfo() = default;
        EventInfo(int fd)
            : fd{fd}, rout{}
            , last_activity{std::chrono::system_clock::now()} {}
        EventInfo(int fd, Coro::RoutineFunc&& func) 
            : fd{fd}, rout{std::move(func)}
            , last_activity{std::chrono::system_clock::now()} {}
        ~EventInfo() = default;

        int fd = -1;

        Coro::Routine rout;
        Client client;

        timer last_activity;
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

        int wait(Events& events);

        void setTimeout(int usec);
        void setTimeout(std::chrono::milliseconds usec);

        EventInfo* setObserve(EventInfo* socket, uint32_t mode); 
        void modObserve(EventInfo* socket, uint32_t mode);
        void delObserve(EventInfo* scoket);

    private:
        int epoll_fd_;
        mutable int timeout_usec_;
};

}   // namespace Network::Services 

#endif  // BASE_SERVICE_H_
