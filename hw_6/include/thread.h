#ifndef THREAD_H_ 
#define THREAD_H_

#include <thread>
#include <algorithm>
#include <functional>

#include "non_copyable.h"
#include "base_service.h"

namespace Network::Thread {


class Thread {
    public:
        Thread(std::size_t max_e, std::function<void(Services::Events&)> func) 
            : events_(max_e)
            , thread_(func, std::ref(events_)) {}

        Thread(Thread&& t) 
            : events_(std::move(t.events_))
            , thread_(std::move(t.thread_)) {}

        Thread& operator=(Thread&& t) {
            if (this == &t) {
                return *this;
            }
            events_ = std::move(t.events_);
            thread_ = std::move(t.thread_);
        } 
        
        void join() {
            thread_.join();    
        }

        void detach() {
            thread_.detach();
        }

        ~Thread() = default;

    private:
        Services::Events events_;
        std::thread thread_;
};

using Threads = std::vector<Thread>;

class ThreadPool {
    public: 
        ThreadPool(std::size_t thread_count, std::size_t max_e,
                   std::function<void(Services::Events&)> func) {
            for (int i = 0; i < thread_count; i++) {
                threads_.emplace_back(max_e, func);
            }
        }

        ~ThreadPool() {
            std::for_each(threads_.begin(), threads_.end(), [](auto& thread){
                thread.join();
            });
        }

    private:
        Threads threads_;
};

}  // namespace Network::Thread

#endif  // THREAD_H_
