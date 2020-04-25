#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>
#include <vector>
#include <algorithm>
#include <string>

#include <chrono>
#include <thread>

#include <sys/types.h>
#include <sys/wait.h>

#include "shared_map.h"

using namespace shm;
using namespace std::chrono_literals;

using String = std::basic_string<char, std::char_traits<char>, Allocator<char>>;

const int FIRST = 100;
const int SECOND = 200;

int main() {
    Map<String, String> map; 

    pid_t pid = fork();
    if (pid != 0) {
        for (int i = 0; i < FIRST; i++) {
            std::this_thread::sleep_for(1ms);
            std::cout << i << " ";
            map.insert({String(std::to_string(i)), String("first")});
        }
    } else {
        for (int i = FIRST; i < SECOND; i++) {
            std::this_thread::sleep_for(1ms);
            std::cout << i << " ";
            map.insert({String(std::to_string(i)), String("second")});
        }

        return 0;
    }

    int p;
    wait(&p);


    std::for_each(map.begin(), map.end(), [](auto& x){
        if (x.second == "first") {
            std::cout << x.first << " : " << x.second << " | ";     
        }
    });
    
    std::for_each(map.begin(), map.end(), [](auto& x){
        if (x.second == "second") {
            std::cout << x.first << " : " << x.second << " | ";     
        }
    });

    return 0;
}
