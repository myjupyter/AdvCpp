#include <iostream>

#include <connection_tcp.h>

#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    try {
        if (argc < 3) {
            throw std::runtime_error(std::string("Should be: ") + \
                    argv[0] + std::string(" ip address + port"));
        }

        Network::ConnectionTcp con({argv[1], 
                static_cast<uint16_t>(std::stoi(argv[2]))});
        if (con.connect() == Network::Socket::OK) {
            
            con.setBlocking(false);
            
            std::string s("0xdeadbeef");
            
            while(!s.empty()) {
                s.clear();
                std::cin >> s;
                
                std::string res;
                res.resize(s.size());

                con.write(s.data(), s.size());
                con.read(res.data(), res.size());
                std::cout << res << std::endl;
            }
        }
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }
    return 0;
} 
