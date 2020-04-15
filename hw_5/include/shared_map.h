#ifndef SHARED_MAP_H_
#define SHARED_MAP_H_


#include <map>
#include <utility>

#include "allocator.h"
#include "semaph.h"

namespace shm {

// configs
static const int kPages = 4;
static const std::size_t kBytes = kPages * ::sysconf(_SC_PAGE_SIZE);


template <
    class Key,
    class T,
    class Compare = std::less<Key>,
    class Alloc = Allocator<std::pair<const Key, T> > 
> class Map {
    public:
        
        using map_type        = std::map<Key, T, Compare, Alloc>;
        using value_type      = std::pair<const Key, T>;
        using allocator_type  = Allocator<value_type>; 
        using size_type       = std::size_t;

    public:
        Map();
       /*explicit Map(const Compare& comp,
                     const Allocator& alloc = Allocator()) = default;
        explicit Map(const Allocator& alloc) = default;
        Map(const Map& map) = default;
        Map(const Map& map, const Allocator& alloc) = default;
        Map(Map&& map) = default;
        Map(Map&& map, const Allocator& alloc) = default;
        */
        ~Map() {
            map_->~map(); 
        }

        
        allocator_type get_allocator() const {
            return map_->get_allocator();
        }


        //Iterators
        map_type::iterator begin() {
            return map_->begin();
        }
        map_type::iterator end() {
            return map_->end();
        }


        // Elements access 
        T& at(const Key& key) {
            SemaphoreLock lock(semaph_);
            return map_->at(key);
        }

        const T& at(const Key& key) const {
            SemaphoreLock lock(semaph_);
            return map_->at(key);
        } 
       
        T& operator[](const Key& key) {
            SemaphoreLock lock(semaph_);
            return map_->operator[](key);
        }

        T& operator[](Key&& key) {
            SemaphoreLock lock(semaph_);
            return map_->operator[](key);
        }

        //Modifiers
        auto insert(const value_type& value) {
            SemaphoreLock lock(semaph_);
            return map_->insert(std::forward<value_type>(value));
        }

        template <typename P>
        auto insert(P&& value) {
            SemaphoreLock lock(semaph_);
            return map_->insert(std::forward<P>(value));
        }
        
        auto insert(value_type&& value) {
            SemaphoreLock lock(semaph_);
            return map_->insert(std::forward<value_type>(value));
        }

        void erase(map_type::iterator pos) {
            SemaphoreLock lock(semaph_);
            map_->erase(pos);
        }
        
        size_type erase(const Key& key) {
            SemaphoreLock lock(semaph_);
            return map_->erase(key);
        }

        void clear() noexcept {
            SemaphoreLock lock(semaph_);
            return map_->clear(); 
        }



    private:
        shared_shptr<value_type> memory_;
        
        map_type* map_;
        Semaphore semaph_;
};

// Constructors 
template <
    class Key,
    class T,
    class Compare,
    class Alloc 
> Map<Key, T, Compare, Alloc>::Map() 
    : memory_(makeShmem<value_type>(kBytes)) {
    SharedMemory mem(reinterpret_cast<void*>(memory_.get()), kBytes);
    Allocator<value_type> alloc(mem);
    
    Semaphore sem(mem.getSemPtr(), 1, true);    
    semaph_ = std::move(sem);

    map_ = new (mem.takeMemory(sizeof(map_type))) map_type{alloc};
}

/*
// Iterators
template <
    class Key,
    class T,
    class Compare,
    class Alloc 
> map::iterator 
Map<Key, T, Compare, Alloc>::begin() {
    return map_->begin();    
}

template <
    class Key,
    class T,
    class Compare,
    class Alloc 
> map::iterator 
Map<Key, T, Compare, Alloc>::end() {
    return map_->end();    
}

// Modifiers
template <
    class Key,
    class T,
    class Compare,
    class Alloc 
> void 
Map<Key, T, Compare, Alloc>::insert(const value_type& value) {
    return map_->insert(value);
}
*/
}  // namespace shm

#endif  // SHRED_MAP_H_
