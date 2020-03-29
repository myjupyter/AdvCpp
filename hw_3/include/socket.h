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
        Socket();
        
        explicit Socket(int socket);
        virtual ~Socket();
    
        std::size_t write(const void* data, std::size_t size);
        void writeExact(const void* data, std::size_t size);

        virtual std::size_t read(void* data, std::size_t size);
        virtual void readExact(void* data, std::size_t size);

        int getSocket() const;

        bool isOpened() const;

    private:
        int sock_;
        bool state_; 
};

}  // namespace Network

#endif  // SOCKET_H_