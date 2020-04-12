#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <new>

#include "shmem.h"
#include "Semaphore.h"

using namespace SharedMemory;

int main() {
    auto ptr = makeShmem<char>(1024);

    pid_t pid = fork();

    if (pid == 0) {
        Semaphore* s = reinterpret_cast<Semaphore*>(ptr.get());
        
        
        s->wait();
        s->release();
    } else {
        Semaphore* sem = new (ptr.get()) Semaphore(1, true);
        sem->wait();
        sem->release();
    }
 
    return 0;
}
