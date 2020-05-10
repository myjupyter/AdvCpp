#include "http_server.h"

#include <iostream>


#include "client_tcp_ecxep.h"
#include "thread.h"

#define SERVER_FLAGS EPOLLIN | EPOLLET | EPOLLONESHOT
#define EPOLL_FLAGS  EPOLLIN | EPOLLET | EPOLLONESHOT 

#define MAX_CONNECTION 0xffff

namespace Network::Services {

CallBack defaultHanlder = [] (Client& client_and_data) {
    auto& [client, package] = client_and_data; 
    std::string buffer;
    try {
        client >> package;
    } catch (std::system_error& err) {
        if (err.code().value() == EAGAIN) { 
            Coro::yield();
            client << package;
        } else {
            std::throw_with_nested(err);
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
        service_.setObserve(new EventInfo{server_->getSocket()}, SERVER_FLAGS);  
        service_.setTimeout(100); 
}


void HttpServer::work() {
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }
    
    Thread::Thread thread1(0xff, [this] (Events& events) {
            while (server_->isOpened()) {
            int n = service_.wait(events);

            std::for_each_n(events.begin(), n, [this] (Event& event) {
                    EventInfo* socket = event.getEventInfo();
                    auto& fd = socket->fd;
                    auto& routine = socket->rout;
                    auto events = event.getMode();

                    if (fd == server_->getSocket()) {
                        std::cout <<"Make conn: " << std::this_thread::get_id() << std::endl;
                        std::cout << "Sock: " <<  socket << std::endl;
                        makeConnection();
                        std::cout << "Sock afta: "<< socket << std::endl;
                        service_.modObserve(socket, SERVER_FLAGS);
                        std::cout <<"Exit make conn: " << std::this_thread::get_id() << std::endl;
                    } else {
                        std::cout <<"Data: " << std::this_thread::get_id() << std::endl;
                        if (!client_pool_.contains(fd)) {
                            return;
                        }
                        try {
                            routine.resume();
                            service_.modObserve(socket, EPOLL_FLAGS);
                        } catch (std::system_error& err) {
                            std::cerr << err.what() << std::endl;
                            deleteConnection(socket);
                        } catch (Exceptions::ClientDisconnect& err) {
                            std::cerr << err.what() << std::endl;
                            deleteConnection(socket);
                        } catch (...) {
                            deleteConnection(socket);
                        }
                        std::cout <<"exit data: " << std::this_thread::get_id() << std::endl;
                    }
            });
        }
    });

     Thread::Thread thread2(0xff, [this] (Events& events) {
            while (server_->isOpened()) {
            int n = service_.wait(events);

            std::for_each_n(events.begin(), n, [this] (Event& event) {
                    auto socket = event.getEventInfo();
                    auto& fd = socket->fd;
                    auto& routine = socket->rout;
                    auto events = event.getMode();

                    if (fd == server_->getSocket()) {
                        std::cout <<"Make conn: " << std::this_thread::get_id() << std::endl;
                        makeConnection();
                        service_.modObserve(socket, SERVER_FLAGS);
                        std::cout <<"Exit make conn: " << std::this_thread::get_id() << std::endl;
                    } else {
                        std::cout <<"Data: " << std::this_thread::get_id() << std::endl;
                        if (!client_pool_.contains(fd)) {
                            return;
                        }
                        try {
                            routine.resume();
                            service_.modObserve(socket, EPOLL_FLAGS);
                        } catch (std::system_error& err) {
                            std::cerr << err.what() << std::endl;
                            deleteConnection(socket);
                        } catch (Exceptions::ClientDisconnect& err) {
                            std::cerr << err.what() << std::endl;
                            deleteConnection(socket);
                        } catch (...) {
                            deleteConnection(socket);
                        }
                        std::cout <<"exit data: " << std::this_thread::get_id() << std::endl;
                    }
            });
        }
    });

    thread1.join();
    thread2.join();
}

void HttpServer::makeConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    Client client_and_package; 

    ConnectionTcp& new_client = client_and_package.first.getCon();

    server_->accept(new_client);
    int fd = new_client.getSocket();

    new_client.setBlocking(false);
    
    client_pool_.insert({fd, std::move(client_and_package)});
    
    EventInfo* ei = service_.setObserve(new EventInfo{fd,std::bind(handler_, std::ref(client_pool_[fd]))}, EPOLL_FLAGS);
    
    #ifdef DEBUG
    std::clog << "Connected: " 
              << fd 
              << std::endl;
    #endif
}

void HttpServer::deleteConnection(EventInfo* socket) {
    std::lock_guard<std::mutex> lock(mutex_);
 
    this->service_.delObserve(socket);
    client_pool_.erase(socket->fd);    

    #ifdef DEBUG
    std::clog << "Still connected client: " 
              << client_pool_.size() 
              << std::endl;
    #endif    
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
