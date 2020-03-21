#include <iostream> 
#include <vector>

#include "process.h"


int main(int argc, char *argv[]) { 
    try {
        if (argc < 2) {
            std::string arg(argv[0]);
            throw std::invalid_argument("Should be: " + arg + " {path to bin file}");
        } 

        std::vector<std::string> args(argc - 2);
        for (int i = 2; i < argc; ++i) {
            args[i - 2] = argv[i];
        }    

        process::Process proc(argv[1], args);

        std::string s("0xdeadbeef");
        while (proc.isWorking() && !s.empty()) {
            s.clear();
            std::cin >> s;
            
            std::vector<char> vec(s.size());

            proc.writeExact(s.c_str(), s.size());
            proc.readExact(vec.data(), vec.size());

            vec.push_back('\0');            
            std::cout << vec.data() << std::endl;    
        }
    
    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        return -1;
    } catch (std::invalid_argument& err) {
        std::cout << err.what() << std::endl;
        return -2;
    }
    return 0;
}
