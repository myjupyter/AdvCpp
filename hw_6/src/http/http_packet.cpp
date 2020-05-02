#include "http_packet.h"

#include <algorithm>

std::string HEADER_END = "\n\r";
std::string EMPTY_STRING = "";

namespace Network::Http {

HttpPacket::Headers::iterator HttpPacket::begin() {
    return headers_.begin();
}

HttpPacket::Headers::iterator HttpPacket::end() {
    return headers_.end();
}
        
void HttpPacket::addHeader(const Header& header) {
    headers_.insert(header);
}

std::optional<std::string> HttpPacket::getHeader(const std::string& header) {
    auto it = headers_.find(header);
    if (it != headers_.end()) {
        return it->second;
    }
    return {};
}

/*
std::string HttpPacket::operator[](const std::string& header) {
    return headers_[header];
}
        
std::string& HttpPacket::operator[](const std::string& header) {
    return headers_[header];
}*/

void HttpPacket::setBody(std::string&& body) {
    body_ = std::move(body);
}

void HttpPacket::setBody(const std::string& body) {
    body_ = body;
}
        
std::string& HttpPacket::getBody() {
    return body_;
}

void HttpPacket::setMethod(std::string method) {
    method_ = method;    
}

std::optional<std::string> HttpPacket::getMethod() const {
    return method_;
}

void HttpPacket::setCode(Code code) {
    code_ = code;
}

std::optional<Code> HttpPacket::getCode() const {
    return code_;
}

std::string HttpPacket::toRequest() {   
    std::string http_packet("");
    try {
        http_packet = method_.value() + " " + uri_.value() + "HTTP/" + std::to_string(version_.value());
        http_packet += HEADER_END;
    } catch (std::bad_optional_access& err) {
        throw std::runtime_error("Bad formed request packet: method or uri or HTTP version are absent!");
    }
    std::for_each(headers_.begin(), headers_.end(), [&http_packet](auto& header){
        http_packet += header.first + ": " + header.second + HEADER_END;        
    });

    http_packet += EMPTY_STRING + HEADER_END;

    if (method_.value() != Method::HEAD) {
        http_packet += body_;
    }
    return http_packet;
}

std::string HttpPacket::toResponse() {
    std::string http_packet("");
    try {
        http_packet = "HTTP/" + std::to_string(version_.value()) + " " + std::to_string(static_cast<int>(code_.value()));
        http_packet += HEADER_END;
    } catch (std::bad_optional_access& err) {
        throw std::runtime_error("Bad formed response packet: response code or HTTP version are absent!");
    }
    std::for_each(headers_.begin(), headers_.end(), [&http_packet](auto& header){
        http_packet += header.first + ": " + header.second + HEADER_END; 
    });

    http_packet += EMPTY_STRING + HEADER_END;

    if (body_.empty()) {
        return http_packet;
    }
    return http_packet + body_;
}

}; // namespace Network::Http
