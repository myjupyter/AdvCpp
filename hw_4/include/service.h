#ifndef SERVICE_H_
#define SERVICE_H_

#include <unordered_map>
#include <memory>
#include <functional>
#include <utility>

#include "server_tcp.h"
#include "client_tcp.h"
#include "base_service.h"
#include "ip_address.h"

namespace Network::Services {

using Server     = std::unique_ptr<ServerTcp>;
using Client     = std::pair<ClientTcp, BytePackage>;
using ClientPool = std::unordered_map<int, Client>;
using CallBack   = std::function<void(Client&)>;

class Service : NonCopyable {
    public:
        Service();
        Service(const IpAddress& address, CallBack handler_ = nullptr);
        ~Service() = default;

        void work();
        void stop();

        void setHandler(std::function<void(Client&)>);

    private:
        void makeConnection();
        void deleteConnection(int fd);

        Server server_;
        ClientPool client_pool_;
        BaseService service_;
        CallBack handler_;
                
};

}   // namespace Network::Services 

#endif  // SERVICE_H_
