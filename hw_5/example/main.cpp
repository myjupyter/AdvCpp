#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>
#include <vector>
#include <algorithm>

#include <sys/types.h>
#include <sys/wait.h>

#include "shared_map.h"

using namespace shm;

int main() {
    Map<int, int> map; 

    pid_t pid = fork();
    if (pid != 0) {
        map.insert({1, 4});
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

    return 0;
}
