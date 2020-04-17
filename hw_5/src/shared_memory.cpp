#include "shared_memory.h"


namespace shm {

SharedMemory::SharedMemory()
    : ptr_(nullptr)
    , offset_(0) {}

SharedMemory::SharedMemory(void* ptr, std::size_t n_byte)
    : ptr_(ptr) {
    std::size_t* p = reinterpret_cast<std::size_t*>(ptr_);
    // offset size 
    offset_ = 2 * sizeof(std::size_t) + sizeof(sem_t);
    // size of memory
    *p = n_byte - offset_;                
    // free memory size
    *(p + 1) = *p;

    // pointer to semaphore
    auto* semaphore_ptr = new (reinterpret_cast<sem_t*>(p + 2)) sem_t;
    if (-1 == ::sem_init(semaphore_ptr, 1, 1)) {
        throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)));
    }       
}

SharedMemory::SharedMemory(const SharedMemory& shared_mem) 
    : ptr_(shared_mem.ptr_)
    , offset_(shared_mem.offset_) {}

SharedMemory& SharedMemory::operator=(const SharedMemory& shared_mem) {
    ptr_    = shared_mem.ptr_;
    offset_ = shared_mem.offset_;

    return *this;
}

SharedMemory::SharedMemory(SharedMemory&& shared_mem) 
    : ptr_(std::move(shared_mem.ptr_))
    , offset_(shared_mem.offset_) {}

SharedMemory& SharedMemory::operator=(SharedMemory&& shared_mem) {
    if (this != &shared_mem) {
        ptr_    = std::move(shared_mem.ptr_);
        offset_ = shared_mem.offset_;   
    }
    return *this;
}

bool SharedMemory::operator==(SharedMemory& shared_map) {
    if (ptr_    != shared_map.ptr_ ||
        offset_ != shared_map.offset_) {
        return false;
    }
    return true;
}

bool SharedMemory::operator!=(SharedMemory& shared_map) {
    if (ptr_    != shared_map.ptr_ ||
        offset_ != shared_map.offset_) {
        return  true;
    }
    return false;
}

std::size_t SharedMemory::getSize() const {
    return *(reinterpret_cast<std::size_t*>(ptr_));
}

std::size_t SharedMemory::getFreeMemory() const {
    return *(reinterpret_cast<std::size_t*>(ptr_) + 1);
}

void* SharedMemory::getStart() {
    return getRawPtr() + offset_;
}

void* SharedMemory::getCurrent() {
    return getStart() + (getSize() - getFreeMemory());
}

void* SharedMemory::getEnd() {
    return getStart() + getSize();
}

sem_t* SharedMemory::getSemPtr() {
    auto* ptr = reinterpret_cast<sem_t*>(reinterpret_cast<std::size_t*>(getRawPtr()) + 2);
    return ptr;
}

void* SharedMemory::takeMemory(std::size_t n_bytes) {
    if (getFreeMemory() < n_bytes) {
        throw std::bad_alloc();
    }
    void* ptr = getStart() + (getSize() - getFreeMemory());
    *(reinterpret_cast<std::size_t*>(ptr_) + 1) -= n_bytes; 
    return ptr;
}

void SharedMemory::freeMemory(std::size_t n_bytes) {
    *(reinterpret_cast<std::size_t*>(ptr_) + 1) += n_bytes;
}

void* SharedMemory::getRawPtr() const {
    return reinterpret_cast<void*>(ptr_);
}
 
}  // shm
