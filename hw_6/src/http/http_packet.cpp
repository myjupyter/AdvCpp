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

const std::regex correct_request("(GET|POST|PUT|DELETE|HEAD|CONNECT|OPTIONS|PATCH)( )+(/|\\w)+( )+HTTP/\\d\\.\\d");
const std::regex correct_response("HTTP/\\d.\\d( )+\\d{3}( )?( |\\w?)+");

namespace Network::Http {

// HttpHeader
HttpHeader::HttpHeader(const std::string& head) {
    if (head.empty()) {
        throw Exceptions::HttpPacketBadPacket("Empty packet WTF mate?");
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
        throw Exceptions::HttpPacketBadPacket("Invalid first line of packet");
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

std::string HttpHeader::toString() {
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

void HttpHeader::insert(const Field& header) {
    headers_.insert(header);
}

std::string& HttpHeader::operator[](const std::string& field_name) {
    return headers_[field_name];
}

void HttpHeader::erase(const std::string& field_name) {
    headers_.erase(field_name);
}


// HttpPacket
HttpPacket::HttpPacket(const std::string& packet)
    : HttpHeader(packet)
    , body_("") {
    auto pos = packet.find(EMPTY_STRING);
    if (pos != std::string::npos) {
        body_ = std::move(std::string(packet.begin() + pos, packet.end()));
    }
}
        
void HttpPacket::setBody(std::string&& body) {
    body_ = std::move(body);
}

void HttpPacket::setBody(const std::string& body) {
    body_ = body;
}
        
std::string& HttpPacket::getBody() {
    return body_;
}

std::string HttpPacket::toString() {
    if (body_.empty()) {
        return HttpHeader::toString();
    }
    return HttpHeader::toString() + HEADER_END + body_;
}


}; // namespace Network::Http
