#include <iostream> 

#include "process.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::string arg(argv[0]);
        throw std::invalid_argument(arg + " {path to bin file}");
    } 
    try {
        process::Process proc(argv[1]);
   
     
        std::cout << proc.write("asfd", 4) << std::endl;
        std::cout << proc.write("asfd", 4) << std::endl;
        std::cout << proc.write("asfd", 4) << std::endl;
        std::cout << proc.write("asfd", 4) << std::endl;
        std::cout << proc.write("asfd", 4) << std::endl;

        char buffer[100] = {};

        proc.readExact(buffer, 20);
        std::cout << buffer << std::endl;

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        return -1;
    }

    return 0;
}
