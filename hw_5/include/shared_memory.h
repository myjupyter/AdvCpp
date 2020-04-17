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
using shared_shptr = std::shared_ptr<Type>;

template <typename Type>
shared_shptr<Type> makeShmem(std::size_t n) {
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

class SharedMemory {
    public:
        SharedMemory();
        SharedMemory(void* ptr, std::size_t n_byte);
        SharedMemory(const SharedMemory& shared_mem);
        SharedMemory& operator=(const SharedMemory& shared_mem);
        SharedMemory(SharedMemory&& shared_mem);
        SharedMemory& operator=(SharedMemory&& shared_mem);
        
        bool operator==(SharedMemory& shared_map);
        bool operator!=(SharedMemory& shared_map);

        ~SharedMemory() = default;

        std::size_t getSize() const;
        std::size_t getFreeMemory() const;
        
        void* getStart();
        void* getCurrent();
        void* getEnd();
        sem_t* getSemPtr();

        void* takeMemory(std::size_t n_bytes);
        void freeMemory(std::size_t n_bytes);

    private:
        void* getRawPtr() const; 

        void*  ptr_;
        std::size_t offset_;
};

}  // shm

#endif  // SHRED_MEMORY_H_

