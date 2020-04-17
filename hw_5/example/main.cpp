#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>
#include <vector>
#include <algorithm>
#include <string>

#include <sys/types.h>
#include <sys/wait.h>

#include "shared_map.h"

using namespace shm;

using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

int main() {

    Map<int, String> map; 

    pid_t pid = fork();
    if (pid != 0) {
        map.insert({1, String("df", map.get_allocator())});
        map.insert({2, String("23", map.get_allocator())});
        map.insert({3, String("543", map.get_allocator())});
    } else {
        map.insert({4, String("asd", map.get_allocator())}); 
        map.insert({5, String("dsakfdsf", map.get_allocator())});
        map.insert({6, String("gff", map.get_allocator())});
        
        return 0;        
    }
    
    int p;
    wait(&p);

    std::for_each(map.begin(), map.end(), [](auto& x){
        std::cout << x.first << " : " << x.second << std::endl;     
    });

    return 0;
}
