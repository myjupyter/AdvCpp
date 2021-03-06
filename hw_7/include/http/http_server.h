#ifndef HTTP_HTTP_SERVER_H_
#define HTTP_HTTP_SERVER_H_

#include <map>
#include <memory>
#include <functional>
#include <utility>
#include <mutex>

#include "signal.h"

#include "server_tcp.h"
#include "client_tcp.h"
#include "base_service.h"
#include "http_packet.h"

using namespace Network::Http;

namespace Network::Services {

using EventInfoPtr = std::unique_ptr<EventInfo>;
using Server       = std::unique_ptr<ServerTcp>;
using EventPool    = std::map<int, EventInfoPtr>;
using CallBack     = std::function<void(Client&)>;

class HttpServer : NonCopyable {
    public:
        HttpServer() = delete;
        explicit HttpServer(const IpAddress& address, CallBack handler = nullptr);
        ~HttpServer();

        void work(std::size_t worker_count, double seconds = 10.0);
        void stop();

        void setHandler(std::function<void(Client&)>);
    
        virtual HttpPacket onRequest(const HttpPacket& request); 

    private:
        void makeConnection(EventInfo* socket);
        void deleteConnection(EventInfo* socket);
        void deleteByTimeout(double seconds);

        std::mutex mutex_;
        Server server_;
        EventPool event_pool_;
        BaseService service_;
        CallBack handler_;        
    
    public:
        static bool signal_flag;
};

bool HttpServer::signal_flag = false;

void signalHandler(int signal) {
    if (signal == SIGTERM ||
        signal == SIGINT) {
        HttpServer::signal_flag = true;        
    }
};

}   // namespace Network::Http

#endif  // HTTP_HTTP_SERVER_H_
