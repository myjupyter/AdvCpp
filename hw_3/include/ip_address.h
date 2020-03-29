#ifndef IP_ADDRESS_H_
#define IP_ADDRESS_H_

#include <string>
#include <stdexcept>

#include <netinet/in.h>

namespace Network {

class IpAddress {
    public:
        IpAddress();
        ~IpAddress() = default;

        explicit IpAddress(uint16_t port);
        explicit IpAddress(const std::string& address, uint16_t port); 
        explicit IpAddress(uint32_t address, uint16_t port);

        void setPort(uint16_t port);
    
        sockaddr_in getSockAddr() const; 
    private:
        bool isValidAddr(const std::string& address) const;
        
        sockaddr_in addr;
};

}

#endif  // IP_ADDRESS_H_
