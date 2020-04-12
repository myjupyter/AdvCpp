#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include <semaphore.h>
#include <cerrno>
#include <system_error>

#include "non_copyable.h"

namespace SharedMemory {

class Semaphore : NonCopyable {
    public:
        Semaphore(int init_value = 1, bool shared = false) {
            if (-1 == ::sem_init(&sem_, static_cast<int>(shared), init_value)) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
            }
        }

        void wait() {
            int result;
            while(-1 == (result = ::sem_wait(&sem_)) && errno == EINTR);
            if (-1 == result) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
            }
        }

        void release() {
            if (-1 == ::sem_post(&sem_)) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
            }
        }

        ~Semaphore() {
            ::sem_destroy(&sem_);
        }

    private:
        sem_t sem_;
};

}  // namesapce SharedMemory

#endif  // SEMAPHORE_H_
