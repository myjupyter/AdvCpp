#ifndef SHARED_MEMORY_H_
#define SHARED_MEMORY_H_

#include <sys/mman.h>
#include <unistd.h>

#include <cerrno>
#include <memory>
#include <functional>
#include <stdexcept>
#include <system_error>

#include <semaph.h>

namespace shm {

template <typename Type>
using inter_proc_mem = std::unique_ptr<Type, std::function<void(Type*)>>;

struct MemInfo {
    std::size_t master_pid;
    std::size_t size;
    std::size_t free_memory;
    Semaphore semaphore;

    MemInfo() = delete;
    MemInfo(std::size_t memory_size, pid_t pid = getpid()) 
        : master_pid(static_cast<std::size_t>(pid)) 
        , size(memory_size - sizeof(MemInfo))
        , free_memory(size)
        , semaphore{1, true} {}

    ~MemInfo() = default;
};

template <typename Type>
inter_proc_mem<Type> makeShmem(std::size_t n) {
    constexpr int PROT = PROT_READ | PROT_WRITE;
    constexpr int MAP  = MAP_SHARED | MAP_ANONYMOUS;

    void* ptr = ::mmap(nullptr, n * sizeof(Type), PROT, MAP, -1, 0);
    if (ptr == MAP_FAILED) {
        throw std::bad_alloc();
    }
    auto destructor = [n](Type* ptr_) { 
        reinterpret_cast<MemInfo*>(ptr_)->~MemInfo();
        ::munmap(ptr_,  n * sizeof(Type));
    };

    ptr = new (ptr) MemInfo{n * sizeof(Type), getpid()};

    return {reinterpret_cast<Type*>(ptr), destructor};
}

}  // shm

#endif  // SHRED_MEMORY_H_

