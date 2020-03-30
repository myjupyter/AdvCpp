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

        IpAddress(uint16_t port);
        IpAddress(const std::string& address, uint16_t port); 
        IpAddress(uint32_t address, uint16_t port);

        IpAddress(const IpAddress& address) = default;
        IpAddress& operator=(const IpAddress& address) = default;

        IpAddress(IpAddress&& address) = default;
        IpAddress& operator=(IpAddress&& address) = default;

        void setPort(uint16_t port);
    
        sockaddr_in& getSockAddr(); 
    private:
        bool isValidAddr(const std::string& address) const;
        
        sockaddr_in addr;
};

}

#endif  // IP_ADDRESS_H_
