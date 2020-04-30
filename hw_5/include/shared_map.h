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


using shared_string = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

template <typename T>
struct rebind {
    using type = std::conditional<std::is_pod<T>::value, T, shared_string>::type;
};

template <
    class Key,
    class T,
    class Compare = std::less<typename rebind<Key>::type>
> class Map : NonCopyable {
    public:
        using false_value_type = std::pair<const Key, T>;
        
        using value_type       = std::pair<const typename rebind<Key>::type, typename rebind<T>::type>; 
        using allocator_type   = Allocator<value_type>;
        
        using rebind_key       = rebind<Key>::type;
        using rebind_t         = rebind<T>::type;

        using map_type         = std::map<typename rebind<Key>::type, typename rebind<T>::type, Compare, allocator_type>;
        using size_type        = std::size_t;

    public:
        Map() 
            : memory_(makeShmem<value_type>(kBytes))
            , alloc(reinterpret_cast<void*>(memory_.get())) {
            semaph_ = alloc.getSemPtr();
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
            if (alloc.getMasterPid() == getpid()) {
                map_->~map(); 
            }
        }

        
        allocator_type get_allocator() const {
            return alloc;
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
            if constexpr (std::is_pod<Key>::value) {
                return map_->operator[](key);
            }
            return T(map_->operator[](std::move(rebind_key{key, alloc})));
        }

        T operator[](Key&& key) {
            SemaphoreLock lock(*semaph_);
            if constexpr (std::is_pod<Key>::value) {
                return map_->operator[](key);
            }
            return T(map_->operator[](std::move(rebind_key{key, alloc})));
        }

        //Modifiers 
        void insert(const false_value_type& value) {
            insert_(std::forward<false_value_type>(value));
        }

        template <typename P>
        void insert(P&& value) {
            insert_(std::forward<false_value_type>(value));
        }
        
        void insert(false_value_type&& value) {
            insert_(std::forward<false_value_type>(value));
        }


        void set(const Key& key, T& value) {
            set_(value_type{key, value}); 
        }

        void set(const false_value_type& value) {
            set_(value);
        }

        void erase(map_type::iterator pos) {
            SemaphoreLock lock(*semaph_);
            map_->erase(pos);
        }
        
        size_type erase(const Key& key) {
            SemaphoreLock lock(*semaph_);
            if constexpr (std::is_pod<Key>::value) {
                return map_->erase(key);
            }
            return map_->erase(rebind_key{key, alloc});
        }

        void clear() noexcept {
            SemaphoreLock lock(*semaph_);
            return map_->clear(); 
        }


    private:
        auto doSomething(const false_value_type& value,
                     std::function<void(const false_value_type&)> case_1,
                     std::function<void(const false_value_type&)> case_2,
                     std::function<void(const false_value_type&)> case_3,
                     std::function<void(const false_value_type&)> case_4) {
            SemaphoreLock lock(*semaph_);
            if constexpr (std::is_pod<rebind_key>::value &&
                          std::is_pod<rebind_t>::value) {
                return case_1(value);
            } 
            if constexpr (std::is_same<Key, std::string>::value &&
                          std::is_pod<T>::value) {
                return case_2(value);            
            }
            if constexpr (std::is_pod<Key>::value &&
                          std::is_same<T, std::string>::value) {
                return case_3(value); 
            }
            if constexpr (std::is_same<Key, std::string>::value &&
                          std::is_same<T,   std::string>::value) {
                return case_4(value);
            }

            throw std::runtime_error(std::string(typeid(Key).name()) + " or " + \
                    std::string(typeid(T).name()) + " should be POD type or std::string");
        }

        void set_(const false_value_type& value) {
            doSomething(value,
                    [this] (const false_value_type& value) { this->map_->operator[](rebind_key{value.first})              = value.second;},
                    [this] (const false_value_type& value) { this->map_->operator[](rebind_key{value.first, this->alloc}) = value.second;},
                    [this] (const false_value_type& value) { this->map_->operator[](rebind_key{value.first})              = rebind_t{value.second, this->alloc};},
                    [this] (const false_value_type& value) { this->map_->operator[](rebind_key{value.first, this->alloc}) = rebind_t{value.second, this->alloc};});
        }
        
        auto insert_(const false_value_type& value) {
            doSomething(value,    
                    [this] (const false_value_type& value) { this->map_->insert(value_type{value.first, value.second});},
                    [this] (const false_value_type& value) { this->map_->insert(value_type{rebind_key{value.first, this->alloc}, value.second});},            
                    [this] (const false_value_type& value) { this->map_->insert(value_type{value.first, rebind_t{value.second, this->alloc}});}, 
                    [this] (const false_value_type& value) { this->map_->insert(value_type{rebind_key{value.first, this->alloc}, rebind_t{value.second, this->alloc}});});
        }
        
        inter_proc_mem<value_type> memory_;
        allocator_type alloc; 
        map_type* map_;
        Semaphore* semaph_;
};

}  // namespace shm

#endif  // SHRED_MAP_H_
