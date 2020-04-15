#include "semaph.h"

namespace shm {

// Semaphore

Semaphore::Semaphore() 
    : sem_(nullptr) {}
    
Semaphore::Semaphore(sem_t* init_addres, int init_value, bool shared) {
    sem_ = init_addres;
    if (-1 == ::sem_init(sem_, static_cast<int>(shared), init_value)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
    }
}

Semaphore::Semaphore(Semaphore&& sem)
    : sem_(sem.sem_) {
    sem.sem_ = nullptr;        
}

Semaphore& Semaphore::operator=(Semaphore&& sem) {
    if (this != &sem) {
        sem_ = sem.sem_;

        sem.sem_ = nullptr;
    }
    return *this;
}

void Semaphore::wait() {
    int result;
    while(-1 == (result = ::sem_wait(sem_)) && errno == EINTR);
    if (-1 == result) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
    }
}

void Semaphore::release() {
    if (-1 == ::sem_post(sem_)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
    }
}

Semaphore::~Semaphore() {
    if (sem_ != nullptr) {
        ::sem_destroy(sem_);
    }
}

// SemaphoreLock 

SemaphoreLock::SemaphoreLock(Semaphore& sem) 
    : sem_(sem) {
    sem_.wait();
}

SemaphoreLock::~SemaphoreLock() {
    sem_.release();
}

}  // namesapce shm
