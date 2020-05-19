#include "http_server.h"

#include <chrono>

#include "client_tcp_ecxep.h"
#include "http_packet_excep.h"
#include "thread.h"
#include "global_log_func.h"
#include "resource_manager.h"
#include "configurator.h"

constexpr int EPOLL_FLAGS             = EPOLLIN | EPOLLET | EPOLLONESHOT;
constexpr int MAX_CONNECTION          = 0xffff;
constexpr std::size_t MAX_HEADER_SIZE = 0xfff;

using namespace std::chrono_literals;

namespace Network::Services {

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_{}
    , handler_(handler) {

        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);


        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);
        server_->listen();
        service_.setTimeout(Config::Configurator::get<int>("Base", "epoll_timeout"));
        
        EventInfo* ei = new EventInfo{server_->getSocket()};

        event_pool_.insert({ei->fd, EventInfoPtr(ei)});  
        service_.setObserve(ei, EPOLL_FLAGS);

        if (handler_ != nullptr) {
            return;
        } 

        handler_ = [this] (Client& client_and_data) {
            auto& [client, package] = client_and_data; 

            ssize_t bytes = client.async_read(package);
            
            std::string buffer;
            // Читаем, пока не встретим конец заголовка
            while (!package.getline(buffer, "\r\n\r\n")) {
                bytes = client.async_read(package);
                // Если встретили EAGAIN - прерываем рутину
                // Возобновит её этот или другой поток
                if (bytes == -1) {
                    Coro::yield();
                }
                // Если длина заголовка очень большая, то отклоняем запрос
                if (package.size() > MAX_HEADER_SIZE) {
                    Log::debug("Too Large Http head {" + std::to_string(package.size()) + "}");
                    
                    package.clear();
                    
                    HttpPacket error;
                    error.makeResponse("1.1", Http::Code::PAYLOAD_TOO_LARGE);
                    error["Server"] = Config::Configurator::get<std::string>("Base", "server_name");
                    error.setBody("413 Entity Too Large");

                    std::string str_error = error.toString();
                    client << str_error;
                    return;
                }
            }

            HttpPacket packet;
            try {
                // Попытка из полученных данных сформировать пакет
                packet << buffer;
                buffer.clear();
            } catch (Exceptions::HttpPacketBadPacket& err) {
                Log::info(err.what());
                
                HttpPacket error;
                error.makeResponse("1.1", Http::Code::BAD_REQUEST);
                error["Server"] = "This Server";
                error.setBody("400 Bad Request");

                std::string str_error = error.toString();
                client << str_error;
                return;
            }

            // Адреc клиента
            if (packet["X-Forwarded-For"].empty()) {
                packet["X-Forwarded-For"] = client.getCon().getInfo().getIp();
            }

            // Если есть контент, то догружаем его
            if (packet.getContentLength() != 0) {
                HttpBody body;

                body.addContent(package.getNBytes(packet.getContentLength()));
                std::size_t diff = packet.getContentLength() - body.size();
                // Пока не будет считана вся размерность
                while (diff != 0) {
                    bytes = client.async_read(package);
                    if (bytes == -1) {
                        Coro::yield();
                    } else {
                        body.addContent(package.getNBytes(diff));                    
                    }
                    diff = packet.getContentLength() - body.size();
                }

                packet.setBody(body.getBody());
            }

            // Ответ на запрос 
            // И посылка данных
            HttpPacket res = onRequest(packet);
            std::string str_res;
            res.toString(str_res);
            
            // Процесс отправки данных
            std::size_t rest = 0, old_rest;
            std::size_t size = str_res.size();
            while(rest != size) {
                old_rest = rest;
                
                ssize_t bytes = client.async_write(static_cast<const char*>(str_res.data()) + rest, size - rest);
                // Словили EAGAIN - прерываем рутину
                // Её продолжит этот или уже друой поток
                if (bytes == -1) {
                    Coro::yield();
                } else if (bytes == 0) {
                    throw std::runtime_error("HttpServer::handler: " + std::to_string(rest) + \
                            "/" + std::to_string(size) + " bytes were sent");
                } else {
                    rest += bytes;
                }
            }
        };
}

HttpServer::~HttpServer() {
    std::for_each(event_pool_.begin(), event_pool_.end(), [this] (auto& con) {
        try {
            service_.delObserve(con.second.get());
        } catch (...) {}

    });
    event_pool_.clear();
    Log::info("Server stopped absolutely");
}

void HttpServer::work(std::size_t worker_count, double seconds) {
    std::size_t max_workers = static_cast<std::size_t>(std::thread::hardware_concurrency());
    worker_count = worker_count > max_workers ? max_workers : worker_count;
    
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    auto s_info = server_->getInfo();
    Log::info("Server has been launched on " + s_info.getIp() + " " + std::to_string(s_info.getPort()));
    
    Thread::ThreadPool threads(worker_count, [this, seconds] () {
            Events events(Config::Configurator::get<int>("Base", "events"));
            
            while (server_->isOpened() && !signal_flag) {

                deleteByTimeout(seconds);
                
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
                            if (routine.isFinished()) {
                                service_.modObserve(socket, EPOLL_FLAGS); 
                            } else {
                                service_.modObserve(socket, EPOLL_FLAGS | EPOLLOUT); 
                            }
                        } catch (Exceptions::ClientDisconnect& err) {
                            deleteConnection(socket);
                            Log::debug("Client disconnected");
                        } catch (std::system_error& err) {
                            Log::error(err.what());
                            deleteConnection(socket);
                        } catch (std::runtime_error& err) {
                            Log::error(err.what());
                            deleteConnection(socket);
                        } catch (...) {
                            deleteConnection(socket);
                            Log::error("Client disconnected. Server processing error!");
                        }
                    }
            });
        }
    }); 
}

void HttpServer::makeConnection(EventInfo* socket) {
    EventInfo* ei = new EventInfo{};
    ConnectionTcp& new_client = ei->client.first.getCon();

    ei->last_activity = std::chrono::system_clock::now();

    server_->accept(new_client);
    new_client.setBlocking(false);

    ei->fd = new_client.getSocket();
    ei->rout.reset(std::bind(handler_, std::ref(ei->client)));
    {
        std::lock_guard<std::mutex> lock(mutex_);
        event_pool_.insert({ei->fd, EventInfoPtr(ei)}); 
    }
    auto info = new_client.getInfo();
    Log::debug("New connection {" + std::to_string(event_pool_.size() - 1) + "} from " + info.getIp() + ":" + std::to_string(info.getPort()));
   
    try {
        service_.setObserve(ei, EPOLL_FLAGS);
        service_.modObserve(socket, EPOLL_FLAGS);
    } catch(...) {}
}

void HttpServer::deleteConnection(EventInfo* socket) {
    std::lock_guard<std::mutex> lock(mutex_);
 
    try {
        service_.delObserve(socket);
    } catch (...) {}
    event_pool_.erase(socket->fd);
}

void HttpServer::deleteByTimeout(double seconds) {
    if (mutex_.try_lock() == false) {
        return;
    }

    for (auto event = event_pool_.begin(); event != event_pool_.end();) {
        auto& routine = event->second->rout;
        auto fd = event->second->fd;
        auto start = event->second->last_activity;
                
        auto end = std::chrono::system_clock::now();

        std::chrono::duration<double> diff = end - start;
        if (diff.count() > seconds &&  server_->getSocket() != fd && !routine.isWorking()) { 

            try {
                service_.delObserve(event->second.get());
            } catch (...) {}
            event = event_pool_.erase(event);

            Log::debug("Client timeout disconnection");
        } else {
            ++event;
        }
    }
    mutex_.unlock();
}

void HttpServer::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto node = event_pool_.extract(server_->getSocket());
    
    server_->close();
    std::for_each(event_pool_.begin(), event_pool_.end(), [this] (auto& con) {
        try {
            EventInfo* ei = con.second.get();
            service_.delObserve(ei);

            ei->client.first.getCon().close(); 
        } catch (...) {}

    });
    event_pool_.clear();

    Log::debug("Server has been stoped");
}

void HttpServer::setHandler(CallBack handler) {
    handler_ = handler;
}

HttpPacket HttpServer::onRequest(const HttpPacket& request) {
    HttpPacket response;        
    
    response.makeResponse("1.1", Code::NOT_FOUND);
    response.setBody("404 Not Found");

    return response;
}    

}  // namespace Network::HttpServers
