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
        client >> buffer;
    } catch (std::system_error& err) {
        if (err.code().value() == EAGAIN) {
           client << buffer;
           return;
            // Пытаемся сформировать HttpPacket
            // Если не получается, то кидаем искючение
            // делаем yield
        } else {
            std::throw_with_nested(err);
        }
    }
    client << buffer;
};

HttpServer::HttpServer()
    : server_(std::make_unique<ServerTcp>(IpAddress("127.0.0.1", 8080)))
    , service_{}
    , handler_(defaultHanlder) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);
        service_.setTimeout(1);
} 

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_{}
    , handler_(handler) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);  
        service_.setTimeout(1); 
}


void HttpServer::work() {
    if (handler_ == nullptr) {
        throw std::runtime_error("Triggered service without handler!");
    }

    while (server_->isOpened()) {
        int n = service_.wait();
        
        // connection processing 
        service_.process(n, [this] (Event& event) {
            int socket = event.getFd();
            auto events = event.getMode();

            if (socket == server_->getSocket()) {
                makeConnection();
            } else {
    //            std::cout << "Thread in data section: " << std::this_thread::get_id() << std::endl;
                if (!client_pool_.contains(socket)) {
                    return;
                }
                try {
                    handler_(client_pool_[socket]);
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
      //          std::cout << "Thread out data section: " << std::this_thread::get_id() << std::endl;
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

    #ifdef DEBUG
    std::clog << "Connected: " 
              << fd 
              << " "
              << new_client.getInfo().getIp() 
              << ":" 
              << new_client.getInfo().getPort() 
              << std::endl;
    #endif

    new_client.setBlocking(false);
    client_pool_.insert({fd, std::move(client_and_package)});
    service_.setObserve(fd, EPOLL_FLAGS);
}

void HttpServer::deleteConnection(int fd) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ConnectionTcp& client = client_pool_[fd].first.getCon();
    
    #ifdef DEBUG
    std::clog << "Client disconnected: " 
              << fd
              << " " 
              << client.getInfo().getIp() 
              << client.getInfo().getPort()
              << std::endl;
    
    std::clog << "Still connected client: " 
              << client_pool_.size() 
              << std::endl;
    #endif
    
    try {
        this->service_.delObserve(fd);
    } catch (std::system_error& err) {}
    client_pool_.erase(fd);
}

void HttpServer::stop() {
    service_.delObserve(server_->getSocket());
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
