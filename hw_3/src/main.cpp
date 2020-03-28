#include <iostream>

#include <ip_address.h>
#include <socket.h>
#include <socket_manager.h>

#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

    try {
    Network::Socket fd(open("example.txt", O_CREAT | O_RDWR));
    
    char buffer[100] = "asdffds";
    fd.write(buffer, 100);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }
    return 0;
} 
