#include "http_server.h"

#include "client_tcp_ecxep.h"
#include "thread.h"
#include "global_log_func.h"

constexpr int EPOLL_FLAGS = EPOLLIN | EPOLLET | EPOLLONESHOT;
constexpr int MAX_CONNECTION = 0xffff;

namespace Network::Services {

CallBack defaultHanlder = [] (Client& client_and_data) {
    auto& [client, package] = client_and_data; 
    std::string buffer;
    try {
        client >> buffer;
    } catch (std::system_error& err) {
        if (err.code().value() != EAGAIN) { 
            std::throw_with_nested(err);
        }
        try {
            HttpPacket pack(buffer);
            Coro::yield();
            std::string res = pack.toString();
            client << res;
        } catch (...) {
            HttpPacket error;
            error.makeResponse("1.1", Http::Code::BAD_REQUEST);
            error["Server"] = "This Server";
            std::string res = error.toString();
            client << res;
            return;
        }
    }
};

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_{}
    , handler_(defaultHanlder) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);
        server_->listen();
        service_.setObserve(new EventInfo{server_->getSocket()}, EPOLL_FLAGS);  
        service_.setTimeout(100);
}

void HttpServer::work(std::size_t worker_count) {
    std::size_t max_workers = static_cast<std::size_t>(std::thread::hardware_concurrency());
    worker_count = worker_count > max_workers ? max_workers : worker_count;
    
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    auto s_info = server_->getInfo();
    Log::info("Server have been launched on " + s_info.getIp() + " " + std::to_string(s_info.getPort()));
    Thread::ThreadPool threads(worker_count, [this] () {
            Events events(0xff);
            
            while (server_->isOpened()) {
                int n = service_.wait(events);

                std::for_each_n(events.begin(), n, [this] (Event& event) {
                    EventInfo* socket = event.getEventInfo();
                    auto& fd = socket->fd;
                    auto& routine = socket->rout;
                    auto events = event.getMode();

                    if (fd == server_->getSocket()) {
                        makeConnection();
                        service_.modObserve(socket, EPOLL_FLAGS);
                    } else {
                        if (!client_pool_.contains(fd)) {
                            return;
                        }
                        try {
                            routine.resume();
                            service_.modObserve(socket, EPOLL_FLAGS); 
                        } catch (Exceptions::ClientDisconnect& err) {
                            deleteConnection(socket);
                            
                            Log::debug(err.what());
                        }
                    }
            });
        }
    });
}

void HttpServer::makeConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    Client client_and_package;

    ConnectionTcp& new_client = client_and_package.first.getCon();

    server_->accept(new_client);
    int fd = new_client.getSocket();

    auto info = new_client.getInfo();

    Log::debug("New connection from " + info.getIp() + ":" + std::to_string(info.getPort()));

    new_client.setBlocking(false);
    
    client_pool_.insert({fd, std::move(client_and_package)});
    
    EventInfo* ei = service_.setObserve(new EventInfo{fd, std::bind(handler_, std::ref(client_pool_[fd]))}, EPOLL_FLAGS);
}

void HttpServer::deleteConnection(EventInfo* socket) {
    std::lock_guard<std::mutex> lock(mutex_);
 
    int fd = socket->fd;
    this->service_.delObserve(socket);
    client_pool_.erase(fd);
}

void HttpServer::stop() {
    //service_.delObserve(server_->getSocket());
    server_->close();

    client_pool_.clear();
}

void HttpServer::setHandler(CallBack handler) {
    handler_ = handler;
}

HttpPacket HttpServer::onRequest(const HttpPacket& request) {
    return HttpPacket();
}    

}  // namespace Network::HttpServers
