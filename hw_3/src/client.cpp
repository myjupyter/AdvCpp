#include <iostream>

#include <connection_tcp.h>

#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    try {
        Network::ConnectionTcp con({"127.0.0.1", 8888});
        if (con.connect() == Network::Socket::OK) {
            
            con.setTimeout(std::chrono::seconds(5), Network::ConnectionTcp::READ);
            
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
