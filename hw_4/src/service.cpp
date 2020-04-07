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
            int n = service_.wait(-1);
           
            // DIY handler
            service_.process(n, [this](Event& event) {
                if (event.getMode() == 0) {
                    return;
                }
                if (event.getFd() == this->server_->getSocket()) {
                    // Accpets client
                    Client new_client;
                    this->server_->accept(new_client);
                    
                    // Sets non-blocking mode and registres client in service
                    new_client.setBlocking(false);
                    this->service_.setObserve(new_client.getSocket(), EPOLLIN | EPOLLOUT);
            
                    // Adds clinet to the pool
                    int fd = new_client.getSocket();
                    this->client_pool_.insert({fd, std::move(new_client)});
                } else {
                    int mode = event.getMode();
                    if (!(mode & EPOLLIN) || !(mode & EPOLLOUT)) {
                        return;
                    }
                    if (this->client_pool_.contains(event.getFd())) {
                        int fd = event.getFd();
                        handler_(client_pool_[fd]);
                        
                        if (!client_pool_[fd].isOpened()) {
                            this->service_.delObserve(fd);

                            client_pool_.erase(fd);
                            std::cout << "Client disconnected "<< fd << std::endl;
//                            for_each(client_pool_.begin(), client_pool_.end(), [](auto x){
 //                               std::cout << x->first() << std::endl;
 //                           });
                        }
                    } else {
                        throw std::runtime_error("You've got some problem");
                    }
                }
            });
        }
    } catch(std::runtime_error& err) {
        throw std::runtime_error(std::string("work: \n\t") + std::string(err.what()));
    }
}

void Service::setHandler(CallBack handler) {
    handler_ = handler;
}

}  // namespace Network::Services
