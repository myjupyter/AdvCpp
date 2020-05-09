#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <unordered_map>
#include <memory>
#include <functional>
#include <utility>
#include <mutex>

#include "server_tcp.h"
#include "client_tcp.h"
#include "base_service.h"
#include "ip_address.h"

#include "http_packet.h"

using namespace Network::Http;

namespace Network::Services {

using Server     = std::unique_ptr<ServerTcp>;
using Client     = std::pair<ClientTcp, BytePackage>;
using ClientPool = std::unordered_map<int, Client>;
using CallBack   = std::function<void(Client&)>;

CallBack defaultHandler;

class HttpServer : NonCopyable {
    public:
        HttpServer();
        explicit HttpServer(const IpAddress& address,
                            CallBack handler_ = CallBack(defaultHandler));
        ~HttpServer() = default;

        void work();
        void stop();

        void setHandler(std::function<void(Client&)>);
    
        virtual HttpPacket onRequest(const HttpPacket& request);        
    private:
        void makeConnection();
        void deleteConnection(int fd);

        std::mutex mutex_;
        Server server_;
        ClientPool client_pool_;
        BaseService service_;
        CallBack handler_;
                
};

}   // namespace Network::HttpServers 

#endif  // HTTP_SERVER_H_