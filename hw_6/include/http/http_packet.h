#ifndef HTTP_HTTP_PACKET_H_
#define HTTP_HTTP_PACKET_H_

#include <utility>
#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>

#include "http_common.h"

namespace Network::Http {

class HttpPacket {
    public:
        using Header  = std::pair<std::string, std::string>;
        using Headers = std::unordered_map<std::string, std::string>;
    
    public:
        HttpPacket() = default;
        HttpPacket(const HttpPacket& packet) = default;
        HttpPacket(HttpPacket&& packet) = default;
        HttpPacket& operator=(const HttpPacket& packet) = default;
        HttpPacket& operator=(HttpPacket&& packet) = default;
        ~HttpPacket() = default;

        Headers::iterator begin();
        Headers::iterator end();

        void addHeader(const Header& header);
        std::optional<std::string> getHeader(const std::string& header);
        //std::string operator[](const std::string& header);
        //std::string& operator[](const std::string& header);

        void setBody(std::string&& body);
        void setBody(const std::string& body);
        std::string& getBody();

        void setMethod(std::string method);
        std::optional<std::string> getMethod() const;

        void setCode(Code code);
        std::optional<Code> getCode() const;

        void setVersion(double ver) {
            version_ = ver;
        }

        std::optional<double> getVersion() const {
            return version_;
        }

        std::string toRequest();
        std::string toResponse();

    private:
        std::optional<std::string> method_;
        std::optional<Code>        code_;
        std::optional<std::string> uri_;
        std::optional<double>      version_; 
        
        Headers headers_;
        std::string body_;
};

}

#endif // HTTP_HTTP_PACKET_H_

