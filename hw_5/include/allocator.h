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

    public:
        Allocator()
            : ptr_(nullptr)
            , offset_(0) {}

        template <typename U>
        Allocator(const Allocator<U>& alloc)
            : ptr_(alloc.ptr_)
            , offset_(alloc.offset_) {}
        
        template <typename U>
        Allocator& operator=(const Allocator<U>& alloc) {
            ptr_    = alloc.ptr_;
            offset_ = alloc.offset_;
            
            return *this;
        }
        
        Allocator(void* ptr) 
            : ptr_(ptr) {
            offset_ = sizeof(MemInfo);
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
        
        pid_t getMasterPid() const {
            return static_cast<pid_t>(getMemInfo()->master_pid);
        }

        Semaphore* getSemPtr() {
            return &(getMemInfo()->semaphore);
        }

    private:
        // Special methods
        std::size_t getSize() const {
            return getMemInfo()->size;
        }
        
        std::size_t getFreeMemory() const {
            return getMemInfo()->free_memory;
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

         void* takeMemory(std::size_t n_bytes) {
            if (getFreeMemory() < n_bytes) {
                throw std::bad_alloc();
            }
            void* ptr = getStart() + (getSize() - getFreeMemory());
            getMemInfo()->free_memory -= n_bytes; 
            return ptr;
        }
        
        void freeMemory(std::size_t n_bytes) {
            getMemInfo()->free_memory += n_bytes;
        }

     public: 
        MemInfo* getMemInfo() const {
            return reinterpret_cast<MemInfo*>(ptr_);
        }

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
