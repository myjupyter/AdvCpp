#ifndef SYSTEM_FILE_CONT_H_
#define SYSTEM_FILE_CONT_H_

#include <sys/mman.h>
#include <vector>

template<typename T>
class Allocator {
    public:
        using value_type = T;
        using pointer    = value_type*;

    public:
        Allocator() {}

        explicit Allocator(int fd, std::size_t offset = 0)
            : fd(fd), offset(offset) {}

        ~Allocator() = default;

        explicit Allocator(const Allocator& alloc)
            : fd(alloc.fd)
            , offset(alloc.offset){}

        Allocator& operator=(const Allocator& alloc) {
            if (this != &alloc) {
                fd = alloc.fd;
                offset = alloc.offset;
            }
            return *this;
        }

        explicit Allocator(Allocator&& alloc) {
            if (this != &alloc) {
                fd = alloc.fd;
                offset = alloc.offset;
            }
        }

        Allocator& operator=(Allocator&& alloc) {
            if (this != &alloc) {
                fd = alloc.fd;
                offset = alloc.offset;
            }
            return *this;
        }

        bool operator==(const Allocator<T>& rhs) {
            return true;
        }
        
        bool operator!=(const Allocator<T>& rhs) {
            return false;
        }

        pointer allocate(std::size_t n) {
            void* ptr = ::mmap(0, n * sizeof(value_type), PROT_WRITE | PROT_READ, MAP_SHARED, fd, offset);
            if (ptr == MAP_FAILED) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),"Allocator::allocate");
            }
            return reinterpret_cast<pointer>(ptr);
        }

        void deallocate(pointer ptr, std::size_t n) {
            ::munmap(reinterpret_cast<void*>(ptr), n);
        }

    private:
        int fd;
        std::size_t offset;
};

struct Data {
    Data() {}
    uint8_t payload[24];
};

struct Pair {
    Pair() {};

    uint64_t key;
    Data    data;
};

struct Index {
    Index() {};

    uint64_t key;
    uint64_t offset;
};

using FVector  = std::vector<Pair, Allocator<Pair>>;
using Iterator = FVector::iterator;
using IVector  = std::vector<Index, Allocator<Index>>;

#endif  // SYSTEM_FILE_CONT_H_
