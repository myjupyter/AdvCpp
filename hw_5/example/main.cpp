#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/wait.h>


#include "shared_memory.h"
#include "semaph.h"
#include "allocator.h"
#include "shared_map.h"

using namespace shm;
using shared_vector = std::vector<int, shm::Allocator<int>>;


using pair       = std::pair<const int, int>;
using shared_map = std::map<int, int, std::less<int>, Allocator<pair> >;

int main() {

    SharedMemory<pair> mem_ptr(4096);
    Map<int, int> map(mem_ptr); 

    pid_t pid = fork();
    if (pid != 0) {
        map.insert({4, 4});
        map.insert({5, 4});
        map.insert({1, 4});
        
        } else {
        map.insert({6, 4});
        map.insert({7, 4});
        map.insert({2, 4});
        
        return 0;        
    }

    int p;
    wait(&p);

    std::for_each(map.begin(), map.end(), [](auto& x){
        std::cout << x.first << ":" << x.second << std::endl;     
    });


/*
    SharedMemory<pair> mem_ptr(4096);

    Allocator<pair> alloc(mem_ptr);

    
//    shared_vector* vec = new (mem_ptr.takeMemory(sizeof(shared_vector))) shared_vector{alloc};
    shared_map* vec = new (mem_ptr.takeMemory(sizeof(shared_map))) shared_map{alloc};

    Semaphore sem(mem_ptr.getSemPtr(), 1, true);    

    pid_t pid = fork();
    if (pid != 0) {
        SemaphoreLock lock(sem);
        vec->insert({0, 0});
        vec->insert({2, 2});
        vec->insert({1, 1});
    } else {
        SemaphoreLock lock(sem);
        vec->insert({3, 3});
        vec->insert({1, 5});
        vec->insert({5, 6});
        
        return 0;        
    }

    int p;
    wait(&p);
   
    std::for_each(vec->begin(), vec->end(), [](auto& x){
        std::cout << x.first <<":" << x.second << std::endl;
    });

    return 0;
  */  
    
    /*    
    int size = 8;

    SharedMemory<int> mem_ptr(4096);

    auto* ptr = mem_ptr.takeMemory(size * sizeof(int));
    int* vec = new (ptr) int[size];

    std::cout << vec << " " << ptr << std::endl; 
    std::cout << mem_ptr.getFreeMemory() << std::endl;

    Semaphore sem(mem_ptr.getSemPtr(), 2, true);    

    pid_t pid = fork();
    if (pid != 0) {
        SemaphoreLock lock(sem);
        for (int i = 0; i < size; ++i) {
            vec[i] = i;
        }
    } else {
        SemaphoreLock lock(sem);
        for (int i = 0; i < size; ++i) {
            vec[i] = i - size;
        }

        return 0;        
    }

    int p;
    wait(&p);
        
    for (int i = 0; i < size; ++i) {
            std::cout << vec[i] << " ";
    }
    return 0;
    */
}
