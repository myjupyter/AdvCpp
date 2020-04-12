#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>
#include <vector>
#include <algorithm>

#include "shmem.h"
#include "Semaphore.h"
#include "Allocator.h"

using namespace SharedMemory;

int main() {
    
   // auto ptr = makeShmem<char>(1024);
/*    auto* p = reinterpret_cast<int*>(Memory.get());
    *p = 1;
    std::cout <<  ::sysconf(_SC_PAGE_SIZE) << std::endl; 
    std::vector<int, Allocator<int>> vec(1024);
    std::cout <<vec[0] << *p << std::endl;
*/

    std::cout <<"Vec1" << std::endl;
    std::vector<std::size_t, Allocator<std::size_t>> vec(3, 1); 
   
    pid_t pid = fork();

    if (pid != 0) {
        sleep(4);
        for_each(vec.begin(), vec.end(), [](auto x){
            std::cout << x << " ";        
        });
    } else {
        vec[0] = 3;
    }
    return 0;
}
