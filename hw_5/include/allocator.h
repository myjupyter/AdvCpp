#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <unistd.h>
#include <cstddef>

#include <new>
#include <memory>
#include <iostream>
#include <utility>

#include "shared_memory.h"

namespace shm {

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

        Allocator(SharedMemory memory)
            : memory_(memory) {}

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
        SharedMemory memory_;
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
