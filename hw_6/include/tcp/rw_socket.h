#ifndef RW_SOCKET_H_
#define RW_SOCKET_H_

#include "socket.h"

namespace Network {

class RWSocket : public Socket {    
    public:        
        RWSocket();
        explicit RWSocket(Socket::SockType socket_type);        
        
        RWSocket(RWSocket&& socket) = default;
        RWSocket& operator=(RWSocket&& socket) = default;
        ~RWSocket() = default;
        
        std::size_t write(const void* data, std::size_t size);
        void writeExact(const void* data, std::size_t size);
        std::size_t read(void* data, std::size_t size);
        void readExact(void* data, std::size_t size);
    
       ssize_t read_non_block(void* data, std::size_t size);


    protected:
        explicit RWSocket(int socket);
};

}  // namespace Network

#endif  // RW_SOCKET_H_
