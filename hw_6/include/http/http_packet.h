#ifndef HTTP_HTTP_PACKET_H_
#define HTTP_HTTP_PACKET_H_

#include <utility>
#include <string>
#include <string_view>
#include <optional>
#include <tuple>
#include <unordered_map>

#include "http_common.h"

namespace Network::Http {

class HttpHeader {
    public:
        using Field        = std::pair<std::string, std::string>;
        using Fields       = std::unordered_map<std::string, std::string>;
        using RequestLine  = std::optional<std::tuple<std::string, std::string>>;
        using ResponseLine = std::optional<Code>;
    
    public:
        HttpHeader() = default;
        HttpHeader(const HttpHeader& head) = default;
        HttpHeader(HttpHeader&& head) = default;
        HttpHeader& operator=(const HttpHeader& head) = default;
        HttpHeader& operator=(HttpHeader&& head) = default;
        ~HttpHeader() = default;

        HttpHeader(const std::string& head);

        virtual std::string toString();
        void makeRequest(const std::string& method,
                         const std::string& uri,
                         const std::string& version);
        void makeResponse(const std::string& version, Code code);
        void insert(const Field& header);        
        std::string& operator[](const std::string& field_name);
        void erase(const std::string& field_name);

    private:
        RequestLine  request_line_;
        ResponseLine response_line_;

        std::string version_;

        Fields headers_;
};

class HttpPacket : public HttpHeader {
    public:
        HttpPacket() = default;
        HttpPacket(const HttpPacket& packet) = default;
        HttpPacket(HttpPacket&& packet) = default;
        HttpPacket& operator=(const HttpPacket& packet) = default;
        HttpPacket& operator=(HttpPacket&& packet) = default;
        ~HttpPacket() = default;

        HttpPacket(const std::string& packet);

        std::string toString() override;

        void setBody(std::string&& body);
        void setBody(const std::string& body);
        std::string& getBody();

    private:
        std::string body_;
};

}

#endif // HTTP_HTTP_PACKET_H_

