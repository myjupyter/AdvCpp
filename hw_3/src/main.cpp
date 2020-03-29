#include <iostream>

#include <connection_tcp.h>

#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

    try {
        Network::ConnectionTcp con;
        std::cout << con.connect(Network::IpAddress(8888)) << std::endl;

        char buffer[100] = "asdfadsasdf";

        con.write(buffer, 100);

    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }
    return 0;
} 
