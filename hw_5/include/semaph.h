#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <semaphore.h>
#include <cerrno>
#include <system_error>

#include "non_copyable.h"

namespace shm {

class Semaphore : NonCopyable {
    public:
        Semaphore();
        Semaphore(sem_t* init_addres, int init_value = 1, bool shared = false);
        Semaphore(Semaphore&& sem);
        Semaphore& operator=(Semaphore&& sem);
        ~Semaphore();
        
        void wait();
        void release();

    private:
        friend class SemaphoreLock;
        sem_t* sem_;
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
