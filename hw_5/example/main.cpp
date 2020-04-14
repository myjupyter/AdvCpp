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

using namespace shm;
using shared_vector = std::vector<int, Allocator<int>>;


int main() {
    SharedMemory<int> mem_ptr(4096);

    Allocator<int> alloc(mem_ptr);
    shared_vector* vec = new (mem_ptr.takeMemory(sizeof(shared_vector))) shared_vector{alloc};

    std::cout << mem_ptr.getFreeMemory() << std::endl;

    Semaphore sem(mem_ptr.getSemPtr(), 1, true);    

    pid_t pid = fork();
    if (pid != 0) {
        SemaphoreLock lock(sem);
        vec->push_back(1);
        vec->push_back(3);
        vec->push_back(5);
        vec->push_back(7);
    
        vec->~vector();
    } else {
        SemaphoreLock lock(sem);
        vec->push_back(2);
        vec->push_back(4);
        vec->push_back(6);

        return 0;        
    }

    int p;
    wait(&p);
   
    std::for_each(vec->begin(), vec->end(), [](auto x){
        std::cout << x << std::endl;        
    });

    return 0;
    
    
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
