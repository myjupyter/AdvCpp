#include <iostream>

#include <connection_tcp.h>

#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {

    try {
        Network::ConnectionTcp con;
        if (con.connect(Network::IpAddress(8888)) == Network::Socket::OK) {
            std::cout << con.isBlocking() << std::endl;        

            std::string s;

            std::cin >> s;
            std::string res;
            res.resize(s.size());

            con.write(s.data(), s.size());
            con.read(res.data(), res.size());
            std::cout << res << std::endl;
        }
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
    }
    return 0;
} 
