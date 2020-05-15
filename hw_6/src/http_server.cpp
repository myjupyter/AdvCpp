#include "http_server.h"

#include <chrono>

#include "client_tcp_ecxep.h"
#include "http_packet_excep.h"
#include "thread.h"
#include "global_log_func.h"

#include "resource_manager.h"

constexpr int EPOLL_FLAGS = EPOLLIN | EPOLLET | EPOLLONESHOT;
constexpr int MAX_CONNECTION = 0xffff;

using namespace std::chrono_literals;

namespace Network::Services {

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_{}
    , handler_(handler) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);
        server_->listen();
        service_.setTimeout(100);
        
        EventInfo* ei = new EventInfo{server_->getSocket()};

        event_pool_.insert({ei->fd, EventInfoPtr(ei)});  
        service_.setObserve(ei, EPOLL_FLAGS);

        if (handler_ != nullptr) {
            return;
        } 

        handler_ = [this] (Client& client_and_data) {
            auto& [client, package] = client_and_data; 
            try {
                client >> package;
            } catch (std::system_error& err) {
                if (err.code().value() != EAGAIN) { 
                    std::throw_with_nested(err);
                }
                try {
                    Coro::yield();

                    HttpPacket pack(package.toString());
                    HttpPacket res = onRequest(pack);

                    std::string str_res = std::move(res.toString());
                    client << str_res;
                } catch (Exceptions::HttpPacketBadPacket& err) {
                    Log::info(err.what());

                    HttpPacket error;
                    error.makeResponse("1.1", Http::Code::BAD_REQUEST);
                    error["Server"] = "This Server";
                    error.setBody("400 Bad Request");

                    std::string str_error = error.toString();
                    client << str_error;
                }
                package.clear();
            }
        };
}

void HttpServer::work(std::size_t worker_count, double seconds) {
    std::size_t max_workers = static_cast<std::size_t>(std::thread::hardware_concurrency());
    worker_count = worker_count > max_workers ? max_workers : worker_count;
    
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    auto s_info = server_->getInfo();
    Log::info("Server has been launched on " + s_info.getIp() + " " + std::to_string(s_info.getPort()));

    Thread::Thread timeout_thread([this, seconds] () {
        while (server_->isOpened()) {

        
            for (auto event = event_pool_.begin(); event != event_pool_.end(); event++) {
                auto start = event->second->last_activity;
                auto end = std::chrono::system_clock::now();
                
                std::chrono::duration<double> diff = end - start;
                if (diff.count() > seconds && server_->getSocket() != event->second->fd) {
                    deleteConnection(event->second.get());
                    break;
                }
            }
        }        
    });

    Thread::ThreadPool threads(worker_count, [this] () {
            Events events(4);
            
            while (server_->isOpened()) {
                int n = service_.wait(events);

                std::for_each_n(events.begin(), n, [this] (Event& event) {
                    EventInfo* socket = event.getEventInfo();
                    auto events = event.getMode();
                    auto& fd = socket->fd;
                    auto& routine = socket->rout;
                    socket->last_activity = std::chrono::system_clock::now();

                    if (fd == server_->getSocket()) {
                        makeConnection(socket);
                    } else {
                        try {
                            routine.resume();
                            if (routine.is_finished_) {
                                service_.modObserve(socket, EPOLL_FLAGS); 
                            } else {
                                service_.modObserve(socket, EPOLL_FLAGS | EPOLLOUT); 
                            }
                        } catch (Exceptions::ClientDisconnect& err) {
                            deleteConnection(socket);
                            
                        } catch (...) {
                            deleteConnection(socket);
                            Log::error("Server processing error!");
                        }
                    }
            });
        }
    });
    timeout_thread.join();
}

void HttpServer::makeConnection(EventInfo* socket) {
    EventInfo* ei = new EventInfo{};
    ConnectionTcp& new_client = ei->client.first.getCon();

    // !! 
    ei->last_activity = std::chrono::system_clock::now();

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
    Log::debug("Client disconnected");
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
    auto [method, uri] = request.getRequestLine();
    
    HttpPacket response;
    
    if (method == Http::Method::GET) {
        std::string req = request.toString();
        Log::debug(req);

        ResourceManager::ResourceManager& res_man = ResourceManager::ResourceManager::getInstance();
        std::string body = res_man.getResource(uri, true);
        if (body.empty()) {
            response.makeResponse("1.1", Code::NOT_FOUND);
            response.setBody("404 Not Found");
        }
        response.makeResponse("1.1", Code::OK);
        response.setBody(std::move(body));

    } else if (method == Http::Method::PUT) { 
        response.makeResponse("1.1", Code::NOT_FOUND);
        response.setBody("This was PUT");        
    } else if (method == Http::Method::POST) {
        response.makeResponse("1.1", Code::NOT_FOUND);
        response.setBody("This was POST");        
    } else if (method == Http::Method::DELETE) {
        response.makeResponse("1.1", Code::NOT_FOUND);
        response.setBody("This was DELETE");
    }

    return response;
}    

}  // namespace Network::HttpServers
