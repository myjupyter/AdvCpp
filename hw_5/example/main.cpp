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

const int FIRST = 10;
const int SECOND = 20;

int main() {
    Map<String, String> map; 

    pid_t pid = fork();
    if (pid != 0) {
        for (int i = 0; i < FIRST; i++)
            map.insert({String(std::to_string(i)), String("first")});
    
    } else {
        for (int i = FIRST; i < SECOND; i++)
            map.insert({String(std::to_string(i)), String("first")});
        
        return 0;        
    }
    
    int p;
    wait(&p);
    
    std::for_each(map.begin(), map.end(), [](auto& x){
        std::cout << x.first << " : " << x.second << " | ";     
    });
    
    return 0;
}
