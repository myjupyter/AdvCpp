#include "service.h"
#include <iostream>

namespace Network::Services {

Service::Service()
    : server_(std::make_unique<ServerTcp>(IpAddress("127.0.0.1", 8080)))
    , service_(0)
    , handler_(nullptr) {} 

Service::Service(const IpAddress& address, CallBack handler) 
    : server_(std::make_unique<ServerTcp>(address))
    , service_(0)
    , handler_(handler) {}


void Service::work() {
    try {
        if (handler_ == nullptr) {
            throw std::runtime_error("Triggered service without handler!");
        }
        server_->setBlocking(false);
        server_->setMaxConnections(0xff);        
        server_->listen();
        service_.setObserve(server_->getSocket(), EPOLLIN);

        while (server_->isOpened()) {
            int n = service_.wait();

            // connection processing 
            service_.process(n, [this](Event& event) {
                int mode = event.getMode();
                int fd = event.getFd();

                if (mode == 0) { 
                    return;
                }
                if (fd == this->server_->getSocket()) {
                    if (!(mode & EPOLLIN)) {
                        return;
                    }
                    this->makeConnection();    
                } else {
                    if (!(mode & EPOLLIN) || !(mode & EPOLLOUT)) {
                        return;
                    }
                    if (this->client_pool_.contains(fd)) {
                        try {
                            handler_(client_pool_[fd]);
                        } catch (std::exception& err) {
                            std::clog << err.what() << std::endl;
                            this->deleteConnection(fd);
                            return;
                        }
                        ConnectionTcp& client = client_pool_[fd].first.getCon();
                        if (!client.isOpened()) {
                            this->deleteConnection(fd);
                        }   
                    } 
                }
            });
        }
    } catch(std::runtime_error& err) {
        throw std::runtime_error(std::string("Service::work:") + std::string(err.what()));
    }
}

void Service::makeConnection() {
    Client client_and_package; 
    ConnectionTcp& new_client = client_and_package.first.getCon();

    this->server_->accept(new_client);

    #ifdef DEBUG
    std::clog << "Connected: " 
              << new_client.getInfo().getIp() 
              << ":" 
              << new_client.getInfo().getPort() 
              << std::endl;
    #endif

    new_client.setBlocking(false);
    this->service_.setObserve(new_client.getSocket(), EPOLLIN | EPOLLOUT);

    int fd = new_client.getSocket();
    this->client_pool_.insert({fd, std::move(client_and_package)});
}

void Service::deleteConnection(int fd) {
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

void Service::stop() {
    service_.delObserve(server_->getSocket());
    server_->close();

    client_pool_.clear();
}

void Service::setHandler(CallBack handler) {
    handler_ = handler;
}

}  // namespace Network::Services
