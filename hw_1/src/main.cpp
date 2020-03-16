#include <iostream> 

#include "process.h"


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::string arg(argv[0]);
        throw std::invalid_argument(arg + " {path to bin file}");
    } 
    try {
        process::Process proc(argv[1]);

        while (proc.isWorking()) {
            std::string s;
            std::cin >> s;
            
            proc.writeExact(s.c_str(), s.size());
            
            char *buffer = new char[s.size() + 1];
            proc.readExact(buffer, s.size());
            std::cout << buffer << std::endl;
            
            delete[] buffer;
        }

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        return -1;
    }

    return 0;
}
