#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <unistd.h>
#include <cstddef>

#include <new>
#include <memory>
#include <iostream>
#include <utility>

#include "shared_memory.h"

static const int kPages = 1;
static const std::size_t kBytes = kPages * ::sysconf(_SC_PAGE_SIZE);

namespace shm {

class Memory {
    public:
        Memory() = delete;
        Memory(void* ptr, std::size_t offset) 
            : ptr_(ptr)
            , offset_(offset) {
            if (ptr_ == nullptr) {
                throw std::invalid_argument("asdf");
            }
        }
        Memory(const Memory& mem) = default;
        Memory& operator=(const Memory& mem) = default;
        Memory(Memory&& mem) 
            : ptr_(mem.ptr_) {
            mem.ptr_ = nullptr;
        }

        Memory& operator=(Memory&& mem) {
            if (this != &mem) {
                ptr_ = mem.ptr_;
                mem.ptr_ = nullptr;     
            }
            return *this;
        }
        ~Memory() = default;

        std::size_t getSize() const {
            return *(reinterpret_cast<std::size_t*>(ptr_));
        }

        std::size_t getFreeMemory() const {
            return *(reinterpret_cast<std::size_t*>(ptr_) + 1);
        }

        void* getStart() {
            return getRawPtr() + offset_;
        }

        void* getEnd() {
            return getStart() + getSize();
        }

        void* getCurrent() {
            return getStart() + (getSize() - getFreeMemory());
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
            *(reinterpret_cast<std::size_t*>(ptr_) + 1) -= n_bytes; 
            return ptr;
        }

        void freeMemory(std::size_t n_bytes) {
            *(reinterpret_cast<std::size_t*>(ptr_) + 1) += n_bytes;
        }

    private:
        void* getRawPtr() const {
            return ptr_;
        }

        void* ptr_; 
        std::size_t offset_;
};

template <typename Type>
class Allocator {
    public:
        using pointer    = Type*;
        using value_type = Type;

        template <typename U>
        struct rebind {
            typedef Allocator<U> other;
        };

    public:

        Allocator(const Allocator<value_type>& alloc)
            : memory_(alloc.memory_) {}
    
        template <typename U>
        Allocator(const Allocator<U>& alloc)
            : memory_(alloc.memory_) {}


        Allocator& operator=(const Allocator<value_type>& alloc) {
            memory_ = alloc.memory_;
            return *this;
        }
        
        template <typename U>
        Allocator& operator=(const Allocator<U>& alloc) {
            memory_ = alloc.memory_;
            return *this;
        }
        
        explicit Allocator(SharedMemory<value_type>& resource)
            : memory_(resource.ptr_.get(), resource.offset_){}
 
        ~Allocator() = default;

        pointer allocate(std::size_t n) {
            std::size_t n_bytes = n * sizeof(value_type);

            if (memory_.getFreeMemory() < n_bytes) {
                throw std::bad_alloc();
            }

            return reinterpret_cast<pointer>(memory_.takeMemory(n_bytes)); 
        }

        void deallocate(pointer p, std::size_t n) {
            if (p == reinterpret_cast<pointer>(memory_.getCurrent()) - n) {
                memory_.freeMemory(n * sizeof(value_type));
            }
        }

    public:
        Memory memory_;
};


//заглушки
template <class Type, class U>
bool operator==(const Allocator<Type>&, 
                const Allocator<U>&) {
    return true;
}
 
template <class Type, class U>
bool operator!=(const Allocator<Type>&, 
                const Allocator<U>&)  {
    return false;
}

}  // namespace sh

#endif  // ALLOCATOR_H_
