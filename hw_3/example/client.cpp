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

        Network::IpAddress a(argv[1], static_cast<uint16_t>(std::stoi(argv[2])));
        Network::ConnectionTcp con({argv[1], static_cast<uint16_t>(std::stoi(argv[2]))});
        
        con.connect(); 
        std::cout << "Connected successfully" << std::endl;
        
        std::string s("0xdeadbeef");
            
        while(!s.empty()) {
            s.clear();
            std::cin >> s;
                
            std::string res;
            res.resize(s.size());

            con.writeExact(s.data(), s.size());
            con.readExact(res.data(), res.size());
            std::cout << res << std::endl;
        }
        std::cout << "Disconnected successfully" << std::endl;
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }
    return 0;
} 
