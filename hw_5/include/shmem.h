#ifndef SHMEM_H_
#define SHMEM_H_

#include <sys/mman.h>
#include <cerrno>
#include <memory>
#include <functional>
#include <stdexcept>
#include <system_error>

#include "non_copyable.h"

namespace SharedMemory {

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
class ShMem : NonCopyable {
    public:
        ShMem<Type>() = delete;

        ShMem(std::size_t n_byte, std::size_t offset = 16)
            : ptr_(makeShmem<Type>(n_byte))
            , offset_(offset) {
                if (offset < 16) {
                    throw std::invalid_argument("asdfa");
                }
            
                std::size_t* p = reinterpret_cast<std::size_t*>(ptr_.get());
                *p = n_byte - 2 * sizeof(std::size_t);
                *(p + 1) = *p;

                working_ptr_ = (reinterpret_cast<std::byte*>(p) + offset);
        }

        ShMem(ShMem&& shared_mem) 
            : offset_(shared_mem.offset_)
            , working_ptr_(shared_mem.working_ptr_) {
                if (this != &shared_mem) {
                    ptr_ = std::move(shared_mem.ptr_);

                    shared_mem.working_ptr_ = nullptr;
                }
        }

        ShMem& operator=(ShMem&& shared_mem) {
            if (this != &shared_mem) {
                offset_      = shared_mem.offset_;
                working_ptr_ = shared_mem.working_ptr_;
                ptr_         = std::move(shared_mem.ptr_); 
                
                shared_mem.working_ptr_ = nullptr;
            }
        }

        virtual ~ShMem() = default;

        std::size_t getSize() const {
            return *(reinterpret_cast<std::size_t*>(ptr_.get()));
        }

        std::size_t getFreeMemory() const {
            return *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1);
        }

    private:
        void takeMemory(std::size_t n_bytes) {
            *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1) -= n_bytes;
        }

        void freeMemory(std::size_t n_bytes) {
            *(reinterpret_cast<std::size_t*>(ptr_.get()) + 1) += n_bytes;
        }

        Type* getMemoryPtr() {
            return reinterpret_cast<Type*>(reinterpret_cast<std::byte*>(working_ptr_) + (getSize() - getFreeMemory()));
        }

        friend class Allocator<Type>;

        unique_shptr<Type> ptr_;
        std::size_t offset_;
        void* working_ptr_;
};

}  // SharedMemory

#endif  // SHMEM_H_

