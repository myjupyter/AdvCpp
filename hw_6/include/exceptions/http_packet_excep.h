#ifndef EXCEPTIONS_HTTP_PACKET_EXCEP_H_
#define EXCEPTIONS_HTTP_PACKET_EXCEP_H_

#include <stdexcept>
#include <string>

namespace Network::Exceptions {

class HttpPacketExcep : public std::exception {
    public:
        HttpPacketExcep() = default;
        HttpPacketExcep(const std::string& msg) 
            : message_(msg) {}
        ~HttpPacketExcep() = default;

        const char* what() const noexcept override {
            return message_.c_str();
        }

    private:
        std::string message_;
};

class HttpPacketBadPacket : public HttpPacketExcep {
    public:
        HttpPacketBadPacket() = default;
        HttpPacketBadPacket(const std::string& msg)
            : HttpPacketExcep(msg) {}
        ~HttpPacketBadPacket() = default;
};

}  // namespace Network::Exceptions

#endif  // EXCEPTIONS_HTTP_PACKET_EXCEP_H_
