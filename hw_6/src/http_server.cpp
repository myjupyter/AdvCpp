#include "http_server.h"

#include <chrono>

#include "client_tcp_ecxep.h"
#include "thread.h"
#include "global_log_func.h"

constexpr int EPOLL_FLAGS = EPOLLIN | EPOLLET | EPOLLONESHOT;
constexpr int MAX_CONNECTION = 0xffff;

using namespace std::chrono_literals;

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
        service_.setTimeout(100);
        
        EventInfo* ei = new EventInfo{server_->getSocket()};

        event_pool_.insert({ei->fd, EventInfoPtr(ei)});  
        service_.setObserve(ei, EPOLL_FLAGS);
}

void HttpServer::work(std::size_t worker_count) {
    std::size_t max_workers = static_cast<std::size_t>(std::thread::hardware_concurrency());
    worker_count = worker_count > max_workers ? max_workers : worker_count;
    
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    auto s_info = server_->getInfo();
    Log::info("Server has been launched on " + s_info.getIp() + " " + std::to_string(s_info.getPort()));

    Thread::ThreadPool threads(worker_count, [this] () {
            Events events(0xff);
            
            while (server_->isOpened()) {
                int n = service_.wait(events);

                std::for_each_n(events.begin(), n, [this] (Event& event) {
                    EventInfo* socket = event.getEventInfo();
                    auto events = event.getMode();
                    auto& fd = socket->fd;
                    auto& routine = socket->rout;

                    if (fd == server_->getSocket()) {
                        makeConnection(socket);
                    } else {
                        try {
                            routine.resume();
                            service_.modObserve(socket, EPOLL_FLAGS); 
                        } catch (Exceptions::ClientDisconnect& err) {
                            deleteConnection(socket);
                            
                            Log::debug(err.what());
                        } catch (...) {
                            deleteConnection(socket);
                            Log::error("Client Disconected. Server processing error!");
                        }
                    }
            });
        }
    });
}

void HttpServer::makeConnection(EventInfo* socket) {
    EventInfo* ei = new EventInfo{};
    ConnectionTcp& new_client = ei->client.first.getCon(); 
    
    server_->accept(new_client);
    new_client.setBlocking(false);

    ei->fd = new_client.getSocket();
    ei->rout.reset(std::bind(handler_, std::ref(ei->client)));
    {
        std::lock_guard<std::mutex> lock(mutex_);
        event_pool_.insert({ei->fd, EventInfoPtr(ei)});
    
        auto info = new_client.getInfo();
        Log::debug("New connection {" + std::to_string(event_pool_.size() - 1) + "} from " + info.getIp() + ":" + std::to_string(info.getPort()));
    }
    try {
        service_.setObserve(ei, EPOLL_FLAGS);
        service_.modObserve(socket, EPOLL_FLAGS);
    } catch(...) {}
}

void HttpServer::deleteConnection(EventInfo* socket) {
    std::lock_guard<std::mutex> lock(mutex_);
 
    try {
        this->service_.delObserve(socket);
    } catch (...) {}
    event_pool_.erase(socket->fd);
}

void HttpServer::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto node = event_pool_.extract(server_->getSocket());
    
    std::for_each(event_pool_.begin(), event_pool_.end(), [this] (auto& con) {
        try {
            EventInfo* ei = con.second.get();
            ei->client.first.getCon().close();
            
            Log::warning("Forced disconnection");
        } catch (...) {}

    });
    server_->close();
    event_pool_.clear();
}

void HttpServer::setHandler(CallBack handler) {
    handler_ = handler;
}

HttpPacket HttpServer::onRequest(const HttpPacket& request) {
    return HttpPacket();
}    

}  // namespace Network::HttpServers
