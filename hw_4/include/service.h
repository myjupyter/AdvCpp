#ifndef SERVICE_H_
#define SERVICE_H_

#include <unordered_map>
#include <memory>
#include <functional>

#include "server_tcp.h"
#include "ip_address.h"
#include "base_service.h"

namespace Network::Services {

using Server     = std::unique_ptr<ServerTcp>;
using Client     = ConnectionTcp;
using ClientPool = std::unordered_map<int, Client>;
using CallBack   = std::function<void(Client&)>;

class Service : NonCopyable {
    public:
        Service();
        Service(const IpAddress& address, CallBack handler_ = nullptr);
        ~Service() = default;

        void work();
        void setHandler(std::function<void(Client&)>);

    private:
        Server server_;
        ClientPool client_pool_;
        BaseService service_;
        CallBack handler_;
                
};

}   // namespace Network::Services 

#endif  // SERVICE_H_
