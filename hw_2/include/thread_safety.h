#ifndef THREAD_SAFETY_H_
#define THREAD_SAFETY_H_

#include <mutex>

namespace Log {

class ThreadSafety {
    public:
        inline void lockMut() { 
            access_.lock();
        }
        
        inline void unlockMut() {
            access_.unlock();
        };

        ThreadSafety() = default;
        ~ThreadSafety() = default;

    private:
        std::mutex access_;
};

}  // namespace Log


#endif  // THREAD_SAFETY_H_
