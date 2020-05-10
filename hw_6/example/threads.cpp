#include "thread.h"
#include <iostream>

using namespace Network::Thread;

int main() {

    ThreadPool thread(4, 4, [](Network::Services::Events& events){
        for (int i  = 0; i < 3; i++) {
            std::cout << i << std::endl;
        }        
    });

    return 0;
}
