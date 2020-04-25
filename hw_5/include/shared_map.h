#ifndef SHARED_MAP_H_
#define SHARED_MAP_H_

#include <map>
#include <utility>
#include <type_traits>

#include "allocator.h"
#include "semaph.h"
#include "non_copyable.h"

namespace shm {

// configs
static const int kThreads = 1;
static const int kPages = 60;
static const std::size_t kBytes = kPages * ::sysconf(_SC_PAGE_SIZE);

template <
    class Key,
    class T,
    class Compare = std::less<Key>
> class Map : NonCopyable {
    public:        
        using value_type      = std::pair<const Key, T>;
        using allocator_type  = Allocator<value_type>; 
        using map_type        = std::map<Key, T, Compare, allocator_type>;
        using size_type       = std::size_t;

    public:
        Map() : memory_(makeShmem<value_type>(kBytes)) {
            Allocator<value_type> alloc(reinterpret_cast<void*>(memory_.get()), kBytes);
                
            semaph_ = new (alloc.getSemPtr()) Semaphore{kThreads, true};
            map_ = new (alloc.allocate(sizeof(map_type))) map_type{alloc};
        }

        Map(Map&& map)
            :  memory_(std::move(map.memory_))
            ,  map_(map.map_)
            ,  semaph_(std::move(map.semaph_)) {
            map.map_ = nullptr;
            map.semaph_ = nullptr;
        }

        Map& operator=(Map&& map) {
            if (this != &map) {
                memory_ = std::move(map.memory_);
                map_    = map.map_;
                semaph_ = std::move(map.semaph_);

                map.map_ = nullptr;
                map.semaph_ = nullptr;
            }
            return *this;
        }
        
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
        T operator[](const Key& key) {
            SemaphoreLock lock(*semaph_);
            return map_->operator[](key);
        }

        T operator[](Key&& key) {
            SemaphoreLock lock(*semaph_);
            return map_->operator[](key);
        }

        //Modifiers
        void set(const Key& key, T& value) {
            SemaphoreLock lock(*semaph_);
            map_->operator[](key) = value;
        }
        
        auto insert(const value_type& value) {
            return insert_(std::forward<value_type>(value));
        }

        template <typename P>
        auto insert(P&& value) {
            return insert_(std::forward<value_type>(value));
        }
        
        auto insert(value_type&& value) {
            return insert_(std::forward<value_type>(value));
        }

        void erase(map_type::iterator pos) {
            SemaphoreLock lock(*semaph_);
            map_->erase(pos);
        }
        
        size_type erase(const Key& key) {
            SemaphoreLock lock(*semaph_);
            return map_->erase(key);
        }

        void clear() noexcept {
            SemaphoreLock lock(*semaph_);
            return map_->clear(); 
        }

    private:
        inline auto insert_(const value_type& value) {
            SemaphoreLock lock(*semaph_);
            if constexpr (std::is_pod<Key>::value &&
                          std::is_pod<T>::value) {
                return map_->insert(value);
            } 
            
            auto alloc = get_allocator();
            if constexpr (std::is_constructible<Key, const Key&, const Allocator<value_type>&>::value &&
                          std::is_pod<T>::value) {
                return map_->insert({Key{value.first, get_allocator()}, value.second});
            }
            if constexpr (std::is_pod<Key>::value &&
                          std::is_constructible<T, const T&, const Allocator<value_type>&>::value) {
                return map_->insert({value.first, T{value.second, get_allocator()}}); 
            }
            if constexpr (std::is_constructible<Key, const Key&, const Allocator<value_type>&>::value &&
                          std::is_constructible<T,   const T&,   const Allocator<value_type>&>::value) {
                return map_->insert({Key{value.first, alloc}, T{value.second, alloc}});
            }
            throw std::runtime_error(std::string(typeid(Key).name()) + " or " + \
                    std::string(typeid(T).name()) + " have no allocator field");
        }

        inter_proc_mem<value_type> memory_;
        
        map_type* map_;
        Semaphore* semaph_;
};

}  // namespace shm

#endif  // SHRED_MAP_H_
