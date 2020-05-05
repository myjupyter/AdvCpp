#include "http_server.h"

#include <iostream>
#include <thread>

#define SERVER_FLAGS EPOLLIN | EPOLLET
#define EPOLL_FLAGS  EPOLLIN | EPOLLET | EPOLLONESHOT 

#define MAX_CONNECTION 0xff

namespace Network::Services {

HttpServer::HttpServer()
    : server_(std::make_unique<ServerTcp>(IpAddress("127.0.0.1", 8080)))
    , service_(0)
    , handler_(nullptr) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);
} 

HttpServer::HttpServer(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_(0)
    , handler_(handler) {
        server_->setBlocking(false);
        server_->setMaxConnections(MAX_CONNECTION);        
        server_->listen();
        service_.setObserve(server_->getSocket(), SERVER_FLAGS);  
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
                std::cout << "Thread make connect: " << std::this_thread::get_id();
                makeConnection();
                std::cout << "Thread made connect: " << std::this_thread::get_id();
            } else {
                std::cout << "Thread in data section: " << std::this_thread::get_id();
                 try {
                    handler_(client_pool_[socket]);   
                 } catch (std::system_error& err) {
                    if (err.code().value() == EAGAIN) {
                        service_.modObserve(socket, EPOLL_FLAGS);
                    } 
                    if (err.code().value() == ECONNRESET) {
                        std::cout << "Nen" << std::endl;
                        deleteConnection(socket);
                    }
                } catch (std::runtime_error& err) {
                    std::cout << "Null bytes: " << err.what()  << std::endl;
                    deleteConnection(socket);
                }
                std::cout << "Thread out data section: " << std::this_thread::get_id();
            }
        });
    }
}
void HttpServer::makeConnection() {
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

}  // namespace Network::HttpServers
