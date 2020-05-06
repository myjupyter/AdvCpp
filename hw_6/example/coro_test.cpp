#include "coro.h"
#include <iostream>


using namespace Network;



void foo(int start, int end) {
    for (int i = start; i < end; i += 2) {
        std::cout << i << std::endl;
        Coro::yield();
    }
}


int main() {

    auto id = Coro::create(1, foo, 1, 10);

    for (int i = 0; i < 10; i += 2) {
        std::cout << i << std::endl;
        Coro::resume(id);
    }
    
    return 0;
}
