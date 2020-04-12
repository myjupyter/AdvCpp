#ifndef SHMEM_H_
#define SHMEM_H_

#include <sys/mman.h>
#include <cerrno>
#include <memory>
#include <functional>
#include <system_error>

#include <iostream>

namespace SharedMemory {

template <typename Type>
using unique_shptr = std::unique_ptr<Type, std::function<void(Type*)>>;

template <typename Type>
unique_shptr<Type> makeShmem(uint64_t n) {
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

}  // SharedMemory

#endif  // SHMEM_H_

