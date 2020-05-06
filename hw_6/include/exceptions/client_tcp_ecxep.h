#ifndef EXCEPTIONS_CLIENT_TCP_EXCEP_H_
#define EXCEPTIONS_CLIENT_TCP_EXCEP_H_

#include <stdexcept>
#include <string>

namespace Network::Exceptions {

class ClientTcpExcept : public std::exception {
    public:
        ClientTcpExcept() = default;
        ClientTcpExcept(const std::string& msg) 
            : message_(msg) {}
        ~ClientTcpExcept() = default;

        const char* what() const noexcept override {
            return message_.c_str();
        }

    private:
        std::string message_;
};

class ClientDisconnect : public ClientTcpExcept {
    public:
        ClientDisconnect() = default;
        ClientDisconnect(const std::string& msg)
            : ClientTcpExcept(msg) {}
        ~ClientDisconnect() = default;

};

}  // namespace Network::Exceptions

#endif  // EXCEPTIONS_CLIENT_TCP_EXCEP_H_
