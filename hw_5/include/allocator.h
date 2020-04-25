#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <unistd.h>
#include <cstddef>

#include <new>
#include <memory>
#include <iostream>
#include <utility>

#include "shared_memory.h"
#include "semaph.h"

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

        Allocator()
            : ptr_(nullptr)
            , offset_(0) {}

        Allocator(const Allocator<value_type>& alloc)
            : ptr_(alloc.ptr_)
            , offset_(alloc.offset_) {}
    
        template <typename U>
        Allocator(const Allocator<U>& alloc)
            : ptr_(alloc.ptr_)
            , offset_(alloc.offset_) {}

        Allocator& operator=(const Allocator<value_type>& alloc) {
            ptr_    = alloc.ptr_;
            offset_ = alloc.offset_;
            
            return *this;
        }
        
        template <typename U>
        Allocator& operator=(const Allocator<U>& alloc) {
            ptr_    = alloc.ptr_;
            offset_ = alloc.offset_;
            
            return *this;
        }

        Allocator(void* ptr, std::size_t n_byte) 
            : ptr_(ptr) {
            std::size_t* p = reinterpret_cast<std::size_t*>(ptr_);
            // offset size 
            offset_ = 2 * sizeof(std::size_t) + sizeof(Semaphore);
            // size of memory
            *p = n_byte - offset_;                
            // free memory size
            *(p + 1) = *p;

            // pointer to semaphore
            auto* semaphore_ptr = reinterpret_cast<Semaphore*>(p + 2);
        }

        ~Allocator() = default;

        pointer allocate(std::size_t n) {
            std::size_t n_bytes = n * sizeof(value_type);

            if (getFreeMemory() < n_bytes) {
                throw std::bad_alloc();
            }

            return reinterpret_cast<pointer>(takeMemory(n_bytes)); 
        }

        void deallocate(pointer p, std::size_t n) {
            if (p == reinterpret_cast<pointer>(getCurrent()) - n) {
                freeMemory(n * sizeof(value_type));
            }
        }

        std::size_t getSize() const {
            return *(reinterpret_cast<std::size_t*>(ptr_));
        }
        
        std::size_t getFreeMemory() const {
            return *(reinterpret_cast<std::size_t*>(ptr_) + 1);
        }
        
        void* getStart() {
            return getRawPtr() + offset_;
        }
        void* getCurrent() {
            return getStart() + (getSize() - getFreeMemory());
        }

        void* getEnd() {
            return getStart() + getSize();
        }

        Semaphore* getSemPtr() {
            return reinterpret_cast<Semaphore*>(reinterpret_cast<std::size_t*>(getRawPtr()) + 2);
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

    public:
        void* getRawPtr() const {
            return reinterpret_cast<void*>(ptr_);
        }

        void*  ptr_;
        std::size_t offset_;
};


template <class Type, class U>
bool operator==(const Allocator<Type>& map_l, 
                const Allocator<U>&    map_r) {
    if (map_l.ptr_    != map_r.ptr_ ||
        map_l.offset_ != map_r.offset_) {
        return false;
    }
    return true;
}
 
template <class Type, class U>
bool operator!=(const Allocator<Type>& map_l, 
                const Allocator<U>&    map_r)  {
    if (map_l.ptr_    != map_r.ptr_ ||
        map_l.offset_ != map_r.offset_) {
        return  true;
    }
    return false;
}

}  // namespace sh

#endif  // ALLOCATOR_H_
