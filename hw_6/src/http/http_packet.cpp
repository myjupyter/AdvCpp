#include "http_packet.h"

#include <algorithm>
#include <sstream>
#include <iterator>
#include <regex>
#include <iostream>
#include <cstring>

#include "http_packet_excep.h"

std::string HEADER_END = "\r\n";
std::string EMPTY_STRING = "\r\n\r\n";

const std::regex correct_request("(GET|POST|PUT|DELETE|HEAD|CONNECT|OPTIONS|PATCH)( )+(/|\\w|\\.)+( )+HTTP/\\d\\.\\d");
const std::regex correct_response("HTTP/\\d.\\d( )+\\d{3}( )?( |\\w?)+");

namespace Network::Http {

// HttpHeader
HttpHeader::HttpHeader(const std::string& head) {
    if (head.empty()) {
        throw Exceptions::HttpPacketBadPacket("Empty HTTP packet");
    }
    
    std::stringstream input(head);

    std::string r_line;
    std::getline(input, r_line);

    for (auto ch: HEADER_END) {
        r_line.erase(std::remove(r_line.begin(), r_line.end(), ch), r_line.end());
    }

    std::stringstream s_r_line(r_line);
    std::istream_iterator<std::string> start(s_r_line);
    std::istream_iterator<std::string> end;

    if (std::regex_search(r_line, correct_request)) {
        response_line_ = std::nullopt;
        
        std::string method = *start++;
        std::string uri    = *start++;
        version_ = std::move(std::string(start->begin() + start->find("/") + 1, start->end()));
        request_line_ = std::make_tuple(method, uri);
    } else if (std::regex_search(r_line, correct_response)) {
        request_line_ = std::nullopt;

        version_ = std::move(std::string(start->begin() + start->find("/") + 1, start->end()));
        response_line_ = static_cast<Code>(std::stoi(*(++start)));
    } else {
        throw Exceptions::HttpPacketBadPacket("Invalid the first line of HTTP packet: " + r_line);
    }

    while (std::getline(input, r_line)) {
        for (auto ch: HEADER_END) {
            r_line.erase(std::remove(r_line.begin(), r_line.end(), ch), r_line.end());
        }
        if (!std::strlen(r_line.c_str())) {
            break;
        }
        auto pos = r_line.find(":");
        if (pos == std::string::npos) {
            throw Exceptions::HttpPacketBadPacket("Bad head field");
        }

        auto begin = r_line.begin();
        headers_.insert({std::string(begin, begin + pos), 
                         std::string(begin + pos + 1, r_line.end())});

    }
}

std::string HttpHeader::toString() const {
    std::string http_head("");
    if (response_line_.has_value()) {
        auto& code = response_line_.value();
        http_head = "HTTP/" + version_ + " " + std::to_string(static_cast<int>(code)) + \
                     " " + CodeMessage[code]; 
    } else if (request_line_.has_value()) {
        auto& [method, uri] = request_line_.value();
        http_head = method + " " + uri + " " + "HTTP/" + version_;
    } else {
        throw Exceptions::HttpPacketBadPacket("Bad formed Http packet");
    }
    
    http_head += HEADER_END;

    std::for_each(headers_.begin(), headers_.end(), [&http_head](auto& field) {
        http_head += field.first + ": " + field.second + HEADER_END;        
    });

    return http_head;
}

void HttpHeader::toString(std::string& head) const {
    if (response_line_.has_value()) {
        auto& code = response_line_.value();
        head = "HTTP/" + version_ + " " + std::to_string(static_cast<int>(code)) + \
               " " + CodeMessage[code]; 
    } else if (request_line_.has_value()) {
        auto& [method, uri] = request_line_.value();
        head = method + " " + uri + " " + "HTTP/" + version_;
    } else {
        throw Exceptions::HttpPacketBadPacket("Bad formed Http packet");
    }
    
    head += HEADER_END;

    std::for_each(headers_.begin(), headers_.end(), [&head](auto& field) {
        head += field.first + ": " + field.second + HEADER_END;        
    });
}

void HttpHeader::makeRequest(const std::string& method,
                             const std::string& uri,
                             const std::string& version) {
    response_line_ = std::nullopt;

    version_ = version;
    request_line_  = std::make_tuple(method, uri); 
}

void HttpHeader::makeResponse(const std::string& version, Code code) {
    request_line_ = std::nullopt;

    version_ = version;
    response_line_ = code;
}

std::string HttpHeader::getVersion() const {
    return version_;
}

std::tuple<std::string, std::string> HttpHeader::getRequestLine() const {
    if (request_line_.has_value()) {
        return request_line_.value();
    }
    return std::make_tuple(std::string(), std::string());
} 

Code HttpHeader::getResponseLine() const {
    if (response_line_.has_value()) {
        return response_line_.value();
    }
    return Code::BAD_CODE;
}

void HttpHeader::insert(const Field& header) {
    headers_.insert(header);
}

std::string& HttpHeader::operator[](const std::string& field_name) {
    return headers_[field_name];
}

std::string HttpHeader::getField(const std::string& field_name) const {
    try {
        return headers_.at(field_name);
    } catch (std::out_of_range& err) {}
    return std::string();
}

void HttpHeader::erase(const std::string& field_name) {
    headers_.erase(field_name);
}

HttpHeader& HttpHeader::operator<<(const std::string& head) {
    *this = std::move(HttpPacket(head));
    return *this;
}

HttpHeader& HttpHeader::operator>>(std::string& head) {
    toString(head);
    return *this;
}

// HttpPacket
HttpPacket::HttpPacket(const std::string& packet)
    : HttpHeader(packet)
    , body_("") {
    auto pos = packet.find(EMPTY_STRING);
    if (pos != std::string::npos) {
        body_ = std::move(std::string(packet.begin() + pos + EMPTY_STRING.size(), packet.end()));
        (*this)["Content-Length"] = std::to_string(body_.size());
    }
}
        
void HttpPacket::setBody(std::string&& body) {
    body_ = std::move(body);
    (*this)["Content-Length"] = std::to_string(body_.size());
}

void HttpPacket::setBody(const std::string& body) {
    body_ = body;
    (*this)["Content-Length"] = std::to_string(body_.size());
}
        
const std::string& HttpPacket::getBody() const {
    return body_;
}

std::string HttpPacket::toString() const {
    if (body_.empty()) {
        return HttpHeader::toString();
    }
    return HttpHeader::toString() + HEADER_END + body_;
}

void HttpPacket::toString(std::string& packet) const {
    HttpHeader::toString(packet);
    if (!body_.empty()) {
        packet += HEADER_END + body_;    
    }
}

HttpPacket& HttpPacket::operator>>(std::string& header) {
    toString(header);
    return *this;
}

HttpPacket& HttpPacket::operator<<(const std::string& header) {
    *this = std::move(HttpPacket(header));
    return *this;
}

std::size_t HttpPacket::getContentLength() const {
    std::string len = getField("Content-Length");
    if (len.empty()) {
        return 0;
    }
    return static_cast<std::size_t>(std::stoi(len.c_str()));
}

}; // namespace Network::Http
