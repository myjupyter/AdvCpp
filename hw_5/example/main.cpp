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
    Map<String, String> map; 

    pid_t pid = fork();
    if (pid != 0) {
        map.insert({String("1"), String("df")});
        map.insert({String("2"), std::move(String("23"))});
        map.insert({String("3"), String("543")});
    } else {
        map.insert({String("4"), String("asd")}); 
        map.insert({String("5"), String("dsakfdsf")});
        map.insert({String("6"), String("gff")});
        
        return 0;        
    }
    
    int p;
    wait(&p);
    
    std::for_each(map.begin(), map.end(), [](auto& x){
        std::cout << x.first << " : " << x.second << std::endl;     
    });
    
    return 0;
}
