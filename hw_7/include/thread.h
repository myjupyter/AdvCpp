#ifndef THREAD_H_ 
#define THREAD_H_

#include <thread>
#include <algorithm>
#include <functional>

#include "non_copyable.h"
#include "base_service.h"

#include <iostream>

namespace Network::Thread {

class Thread {
    public:
        Thread(std::function<void()> func) 
            : thread_(func) {}

        Thread(Thread&& t) 
            : thread_(std::move(t.thread_)) {}

        Thread& operator=(Thread&& t) {
            if (this == &t) {
                return *this;
            }
            thread_ = std::move(t.thread_);
            
            return *this;
        } 
        
        void join() {
            thread_.join();    
        }

        void detach() {
            thread_.detach();
        }

        ~Thread() = default;

    private:
        std::thread thread_;
};

using Threads = std::vector<Thread>;

class ThreadPool {
    public: 
        ThreadPool(std::size_t thread_count,
                   std::function<void()> func) {
            for (int i = 0; i < thread_count; i++) {
                threads_.push_back(std::move(Thread(func)));
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
