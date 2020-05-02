#ifndef HTTP_HTTP_PARSER_H_
#define HTTP_HTTP_PARSER_H_

#include "http_common.h"

namespace Network::Http {

class HttpParser {
    HttpParser();
    HttpParser(const std::string& packet);    
    ~HttpParser() = default;
};

} // namespace Network::Http

#endif  // HTTP_HTTP_PARSER_H_
