#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <stdexcept>

#include <non_copyable.h>

namespace Network {

class Socket : NonCopyable {    
    public:        
        enum SockStatus{
            OK,
            ERROR,
            DISCONNECT,
            RECONNECT,
        };

        Socket();
        explicit Socket(int socket);
        
        Socket(Socket&& socket);
        Socket& operator=(Socket&& socket);
        
        virtual ~Socket() = default;
        
        std::size_t write(const void* data, std::size_t size);
        void writeExact(const void* data, std::size_t size);
        std::size_t read(void* data, std::size_t size);
        void readExact(void* data, std::size_t size);

        int getSocket() const;

        void close();

        bool isBlocking() const;
        void setBlocking(bool to_block);

        bool isOpened() const;

        SockStatus getSocketStatus() const;
    protected:
        void setSocketStatus(Socket::SockStatus);
    
    private:

        int sock_;
        SockStatus state_; 
        bool is_blocking_;
};

}  // namespace Network

#endif  // SOCKET_H_
