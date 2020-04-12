#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <unistd.h>
#include <cstddef>

#include <new>
#include <memory>
#include <iostream>

#include <shmem.h>

namespace SharedMemory {

static const int kPages = 1;
static const std::size_t kBytes = kPages * ::sysconf(_SC_PAGE_SIZE);


template <typename Type>
class Allocator {
    public:
        using pointer    = Type*;
        using value_type = Type;

    public:
        Allocator() = default;
        ~Allocator() = default;

        pointer allocate(std::size_t n) {
            std::size_t n_bytes = n * sizeof(value_type);

            if (memory_.getFreeMemory() < n_bytes) {
                throw std::bad_alloc();
            }
            
            std::cout <<"Alloc"<< memory_.getFreeMemory() << std::endl;
             
            memory_.takeMemory(n_bytes);

            std::cout << memory_.getMemoryPtr() << std::endl;
            return memory_.getMemoryPtr();
        }

        void deallocate(pointer p, std::size_t n) {
            memory_.freeMemory(n * sizeof(Type));
            std::cout <<"Dealloc"<< memory_.getFreeMemory() << std::endl;
        }

    private:
        static ShMem<Type> memory_;
};

template <typename Type>
ShMem<Type> Allocator<Type>::memory_ = std::move(ShMem<Type>{kBytes, 16});

}

#endif  // ALLOCATOR_H_
