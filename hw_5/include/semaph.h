#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <semaphore.h>
#include <cerrno>
#include <system_error>
#include <atomic>

#include "non_copyable.h"

namespace shm {

class Semaphore : NonCopyable {
    public:
        Semaphore() = default;
        Semaphore(int init_value = 1, bool shared = false);
        Semaphore(Semaphore&& sem) = delete;
        Semaphore& operator=(Semaphore&& sem) = delete;
        ~Semaphore();
        
        void lock();
        void unlock();

    private:
        void wait();
        void release();
        
        friend class SemaphoreLock;

        std::atomic<bool> flag_;
        sem_t sem_;
};


class SemaphoreLock {
    public:
        SemaphoreLock() = delete;
        SemaphoreLock(Semaphore& sem);
        ~SemaphoreLock();

    private:
        Semaphore& sem_;
};

}  // namesapce shm

#endif  // SEMAPHORE_H_
