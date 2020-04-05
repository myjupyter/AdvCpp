#ifndef SERVICE_H_
#define SERVICE_H_

#include <vector>
#include <memory>
#include <tuple>
#include <functional>

#include "server_tcp.h"
#include "ip_address.h"

/*using Server     = std::unique_ptr<ServerTcp>;
using ClientPool = std::vector<Network::ConnectionTcp>;
using CallBack   = std::function<void(Connection&)>;
*/
namespace Network::Services {

class Service {
    public:
        Service();
//        Service(const IpAddress& address, C);
        ~Service();

    private:
/*        CallBack handler_;
        Server server_;
        ClientPool client_pool_;
*/
        };

}   // namespace Network::Services 

#endif  // SERVICE_H_
