#ifndef SOCKET_H_
#define SOCKET_H_

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <system_error>

#include "non_copyable.h"
#include "socket_manager.h"

namespace Network {

class Socket : NonCopyable {    
    public:         
        enum SockType {
            TCP,
            UDP
        };
        
        Socket();
        explicit Socket(SockType socket_type); 
        Socket(Socket&& socket);
        Socket& operator=(Socket&& socket);
        virtual ~Socket();
        
        int getSocket() const;
        void close();

        bool isBlocking() const;
        void setBlocking(bool to_block);

        bool isOpened() const;
       
        enum SockStatus{
            OK,
            DISCONNECT,
        };

        SockStatus getSocketStatus() const;
    
    protected:
        explicit Socket(int socket);
        void setSocketStatus(Socket::SockStatus);
    
    private:
        int sock_;
        SockStatus state_; 
        bool is_blocking_;
};

}  // namespace Network

#endif  // SOCKET_H_
