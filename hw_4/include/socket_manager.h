#ifndef SOCKET_MANAGER_H_
#define SOCKET_MANAGER_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <chrono>
#include <stdexcept>

#include "ip_address.h"

namespace Network {

namespace  SocketManager {
        
int makeSocket(int type = SOCK_STREAM);
int bindSocket(int socket, IpAddress& addr);
void listenSocket(int socket, int count);
int accept(int socket, IpAddress& addr);

int setBlocking(int socket);
int setTimeout(int socket, int option, std::chrono::seconds time);
int setOption(int socket, int option);
int getOption(int socket);

}   // SocketManager

}   // Network

#endif  // SOCKET_MANAGER_H_
