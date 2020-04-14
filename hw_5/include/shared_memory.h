#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include <sys/mman.h>
#include <cerrno>
#include <memory>
#include <functional>
#include <stdexcept>
#include <system_error>

#include "non_copyable.h"
#include "semaphore.h"

#include <iostream>

namespace shm {

template <typename Type>
class Allocator;

template <typename Type>
using unique_shptr = std::unique_ptr<Type, std::function<void(Type*)>>;

template <typename Type>
unique_shptr<Type> makeShmem(std::size_t n) {
    constexpr int PROT = PROT_READ | PROT_WRITE;
    constexpr int MAP  = MAP_SHARED | MAP_ANONYMOUS;

    void* ptr = ::mmap(nullptr, n * sizeof(Type), PROT, MAP, -1, 0);
    if (ptr == MAP_FAILED) {
        throw std::bad_alloc();
    }
    auto destructor = [n](Type* ptr_) { 
        ::munmap(ptr_,  n * sizeof(Type));
    };

    return {reinterpret_cast<Type*>(ptr), destructor};
}


template <typename Type>
class SharedMemory : NonCopyable {
    public:
        SharedMemory<Type>() = delete;

        SharedMemory(std::size_t n_byte)
            : ptr_(makeShmem<Type>(n_byte)) {
                std::size_t* p = reinterpret_cast<std::size_t*>(ptr_.get());
                // offset size 
                offset_ = 2 * sizeof(std::size_t) + sizeof(sem_t);
                // size of memory
                *p = n_byte - offset_;                
                // free memory size
                *(p + 1) = *p;
                
                // pointer to semaphore
                auto* semaphore_ptr = reinterpret_cast<sem_t*>(p + 2); 
                semaphore_ptr = new (semaphore_ptr) sem_t;
                if (-1 == ::sem_init(semaphore_ptr, 1, 1)) {
                    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
                }       
        }

        SharedMemory(SharedMemory&& shared_mem) 
            : ptr_(std::move(shared_mem.ptr_)) {}

        SharedMemory& operator=(SharedMemory&& shared_mem) {
            if (this != &shared_mem) {
                ptr_   = std::move(shared_mem.ptr_);     
            }
        }

        virtual ~SharedMemory() = default;

        std::size_t getSize() const {
            return *(reinterpret_cast<std::size_t*>(ptr_.get()));
        }

        std::size_t getFreeMemory() const {
            return *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1);
        }

        void* getStart() {
            return getRawPtr() + offset_;
        }

        void* getEnd() {
            return getStart() + getSize();
        }
        
        sem_t* getSemPtr() {
            auto* ptr = reinterpret_cast<sem_t*>(reinterpret_cast<std::size_t*>(getRawPtr()) + 2);
            std::cout << ptr << std::endl;
            return ptr;
        }
        
        void* takeMemory(std::size_t n_bytes) {
            if (getFreeMemory() < n_bytes) {
                throw std::bad_alloc();
            }
            void* ptr = getStart() + (getSize() - getFreeMemory());
            *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1) -= n_bytes; 
            return ptr;
        }

        void freeMemory(std::size_t n_bytes) {
            *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1) += n_bytes;
        }

    private:
        void* getRawPtr() const {
            return reinterpret_cast<void*>(ptr_.get());
        }

        friend class Allocator<Type>;
        
        unique_shptr<Type> ptr_;
        std::size_t offset_;
};

}  // shm

#endif  // SHRED_MEMORY_H_

