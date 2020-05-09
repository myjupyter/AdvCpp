#include "http_server.h"

#include <iostream>
#include <thread>

#include "client_tcp_ecxep.h"

#define SERVER_FLAGS EPOLLIN | EPOLLET
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

HttpServer::HttpServer()
    : server_(std::make_unique<ServerTcp>(IpAddress("127.0.0.1", 8080)))
    , service_{}
    , handler_(defaultHanlder) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);
        service_.setTimeout(100);
} 

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_{}
    , handler_(handler) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);  
        service_.setTimeout(100); 
}


void HttpServer::work() {
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    while (server_->isOpened()) {
        int n = service_.wait();
        
        service_.process(n, [this] (Event& event) {
            auto socket = event.getEventInfo();
            auto& fd = socket->fd;
            auto& routine = socket->rout;
            auto events = event.getMode();

            if (fd == server_->getSocket()) {
                std::cout <<"Make conn: " << std::this_thread::get_id() << std::endl;
                makeConnection();
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
}

void HttpServer::makeConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    Client client_and_package; 
    ConnectionTcp& new_client = client_and_package.first.getCon();

    server_->accept(new_client);
    int fd = new_client.getSocket();

    new_client.setBlocking(false);
    client_pool_.insert({fd, std::move(client_and_package)});
    EventInfo* ei = service_.setObserve(fd, EPOLL_FLAGS);
    
    ei->rout.reset(std::bind(handler_, std::ref(client_pool_[fd])));

    #ifdef DEBUG
    std::clog << "Connected: " 
              << fd 
              << " "
              << new_client.getInfo().getIp() 
              << ":" 
              << new_client.getInfo().getPort() 
              << std::endl;
    #endif
}

void HttpServer::deleteConnection(EventInfo* socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ConnectionTcp& client = client_pool_[socket->fd].first.getCon();
 
    client_pool_.erase(socket->fd);
    try {
        this->service_.delObserve(socket);
    } catch (std::system_error& err) {}
   
    #ifdef DEBUG
    std::clog << "Client disconnected: " 
              << socket->fd
              << " " 
              << client.getInfo().getIp() 
              << client.getInfo().getPort()
              << std::endl;
    
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
