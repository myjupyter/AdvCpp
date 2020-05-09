#include "coro.h"

#include <iostream>
#include <thread>

using namespace Network::Coro;


auto foo = [] (int& start, int& end) {
    for (int i = start; i < end; i += 2) {
        std::cout << i << std::endl;
        yield();
    }
};

int main() {
    Routine rout(std::bind(foo, std::forward<int>(1), std::forward<int>(10)));

    std::thread t([&rout] () {
            for (int i = 0; i < 10; i += 2) {
                std::cout << i << std::endl;
                rout.resume();
            }
    });

    t.join();
    
    return 0;
}
