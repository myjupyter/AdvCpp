#ifndef CLIENT_TCP_H_
#define CLIENT_TCP_H_

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "connection_tcp.h"
#include "non_copyable.h"

namespace Network {

class BytePackage {
    public:
        BytePackage() = default;
        ~BytePackage() = default;

        BytePackage& operator<<(const std::string& data);
        BytePackage& operator>>(std::string& data);
        bool getline(std::string& data, const std::string& delim); 
        bool getline(std::string& data, const std::string& delim,
                     std::size_t chunk_size);

    private:
        bool hasData() const;
        
        std::string data_;
        std::size_t current_pos_ = 0;
};

class ClientTcp : NonCopyable {
    public:
        ClientTcp() = default;
        ~ClientTcp() = default;  
        ClientTcp(ClientTcp&& client) = default;
        ClientTcp& operator=(ClientTcp&& client) = default;

        ClientTcp& operator>>(std::string& package);
        ClientTcp& operator<<(std::string& package);
        ClientTcp& operator>>(BytePackage& package);
        ClientTcp& operator<<(BytePackage& package);

        const ConnectionTcp& getCon() const; 
        ConnectionTcp& getCon();
    
    private: 
        ConnectionTcp connection_;
};

}  // namespace Network

#endif  //CLIENT_TCP_H_
