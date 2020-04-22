#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include <sys/mman.h>
#include <semaphore.h>

#include <cerrno>
#include <memory>
#include <functional>
#include <stdexcept>
#include <system_error>


namespace shm {

template <typename Type>
using inter_proc_mem = std::unique_ptr<Type, std::function<void(Type*)>>;

template <typename Type>
inter_proc_mem<Type> makeShmem(std::size_t n) {
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

}  // shm

#endif  // SHRED_MEMORY_H_

