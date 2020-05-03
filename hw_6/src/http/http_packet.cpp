#include "http_packet.h"

#include <algorithm>
#include <sstream>
#include <iterator>
#include <regex>
#include <iostream>

std::string HEADER_END = "\n\r";
std::string EMPTY_STRING = "";
    
const std::string_view header_del(":");
const std::regex regex_code("\\d{3}");
const std::regex regex_method("(GET|POST|PUT|DELETE|HEAD|CONNECT|OPTIONS|PATCH)");

const std::regex regex_http("HTTP/\\d.\\d");
const std::regex regex_http_version("\\d\.\\d");

namespace Network::Http {

HttpPacket::HttpPacket(const std::string& packet) {
    if (packet.empty()) {
        std::invalid_argument("Wrong");
    }
    std::stringstream input(packet);
    
    std::string first_line;
    std::getline(input, first_line);

    std::stringstream stream_line(first_line);
    std::istream_iterator<std::string> iter(stream_line);

    if (std::regex_search(first_line, regex_method)) {
        method_ = *(iter++);
        uri_    = *(iter++);

        std::string http_ver = *iter;
        auto slash = http_ver.find("/");
        version_ = std::stof(std::string(http_ver.begin() + slash + 1, http_ver.end()));
    } else {
        std::string http_ver = *iter;
        auto slash = http_ver.find("/");
        version_ = std::stof(std::string(http_ver.begin() + slash + 1, http_ver.end()));

        code_ = static_cast<Code>(std::stoi(*(++iter)));
    }

    for (std::string line; std::getline(input, line);) {
        for (auto c: HEADER_END) {
            line.erase(std::remove(line.begin(), line.end(), c), line.end());
        }
        if (line == "") {
            break;
        }
        auto it = line.find(":");
        auto start = line.begin();
        addHeader({std::string(start, start + it),
                   std::string(start + it + 1, line.end())});
    }
    body_ = std::move(std::string(std::istreambuf_iterator<char>(input), {}));
}

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
        http_packet = method_.value() + " " + uri_.value() + " " + "HTTP/" + std::to_string(version_.value());
        http_packet += HEADER_END;
    } catch (std::bad_optional_access& err) {
        throw std::runtime_error("Bad formed request packet: method or uri or HTTP version are absent!");
    }

    if (method_.value() != Method::HEAD) {
        headers_["Content-Length"] = std::to_string(body_.size());
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
        http_packet = "HTTP/" + HttpVersion[version_.value()] + " " + \
                       std::to_string(static_cast<int>(code_.value())) + " " + \
                       CodeMessage[code_.value()];
        http_packet += HEADER_END;
    } catch (std::bad_optional_access& err) {
        throw std::runtime_error("Bad formed response packet: response code or HTTP version are absent!");
    }
   
    if (!body_.empty()) { 
        headers_["Content-Length"] = std::to_string(body_.size());
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
