#ifndef HTTP_HTTP_COMMON_H_
#define HTTP_HTTP_COMMON_H_

#include <string>
#include <string_view>
#include <unordered_set>
#include <unordered_map>

namespace Network::Http {

static std::unordered_map<double, std::string> HttpVersion = {
    {0.9, "0.9"},
    {1.0, "1.0"},
    {1.1, "1.1"},
    {2.0, "2.0"}
};

namespace Method {
    constexpr std::string_view GET     = "GET";
    constexpr std::string_view HEAD    = "HEAD";
    constexpr std::string_view POST    = "POST";
    constexpr std::string_view PUT     = "PUT";
    constexpr std::string_view DELETE  = "DELETE";
    constexpr std::string_view CONNECT = "CONNECT";
    constexpr std::string_view OPTIONS = "OPTIONS";
    constexpr std::string_view PATCH   = "PATCH";
} // namesapce Method

enum class Code {
    // Informational
    CONTINUE                           = 100,
    SWITCHING_PROTOCOL                 = 101,
    PROCESSING                         = 102,
    
    // Success
    OK                                 = 200,
    CREATED                            = 201,
    ACCEPTED                           = 202,
    NON_AUTHORITATIVE_INFORMATION      = 203,
    NO_CONTENT                         = 204,
    RESET_CONTENT                      = 205,
    PARTIAL_CONTENT                    = 206,
    MULTI_STATUS                       = 207,
    ALREADY_REPORTED                   = 208,
    IM_USED                            = 226,
    
    // Redirection
    MULTIPLE_CHOICES                   = 300,
    MOVED_PERMANENTLY                  = 301,
    FOUND                              = 302,
    SEE_OTHER                          = 303,
    NOT_MODIFIED                       = 304,
    USE_PROXY                          = 305,
    TEMPORARY_REDIRECT                 = 307,
    PERMANENT_REDIRECT                 = 308,
    
    // ClientError 
    BAD_REQUEST                        = 400,
    UNAUTHORIZED                       = 401,
    PAYMENT_REQUIRED                   = 402,
    FORBIDDEN                          = 403,
    NOT_FOUND                          = 404,
    METHOD_NOT_ALLOWED                 = 405,
    NOT_ACCEPTABLE                     = 406,
    PROXY_AUTHENTICATION_REQUIRED      = 407,
    REQUEST_TIMEOUT                    = 408,
    CONFLICT                           = 409,
    GONE                               = 410,
    LENGTH_REQUIRED                    = 411,
    PRECONDITION_FAILED                = 412,
    PAYLOAD_TOO_LARGE                  = 413,
    REQUEST_URI_TOO_LONG               = 414,
    UNSUPPORTED_MEDIA_TYPE             = 415,
    REQUESTED_RANGE_NOT_SATISFIABLE    = 416,
    EXPECTATION_FAILED                 = 417,
    IM_A_TEAPOT                        = 418,
    MISDIRECTED_REQUEST                = 421,
    UNPROCESSABLE_ENTITY               = 422,
    LOCKED                             = 423,
    FAILED_DEPENDENCY                  = 424,
    UPGRADE_REQUIRED                   = 426,
    PRECONDITION_REQUIRED              = 428,
    TOO_MANY_REQUESTS                  = 429,
    REQUEST_HEADER_FIELDS_TOO_LARGE    = 431,
    CONNECTION_CLOSED_WITHOUT_RESPONSE = 444,
    UNAVAILABLE_FOR_LEGAL_REASONS      = 451,
    CLIENT_CLOSED_REQUEST              = 499,
   
    //ServerError
    INTERNAL_SERVER_ERROR              = 500,
    NOT_IMPLEMENTED                    = 501,
    BAD_GATEWAY                        = 502, 
    SERVICE_UNAVAILABLE                = 503,
    GATEWAY_TIMEOUT                    = 504,
    HTTP_VERSION_NOT_SUPPORTED         = 505,
    VARIANT_ALSO_NEGOTIATES            = 506,
    INSUFFICIENT_STORAGE               = 507,
    LOOP_DETECTED                      = 508,
    NOT_EXTENDED                       = 510,
    NETWORK_AUTHENTICATION_REQUIRED    = 511,
    NETWORK_CONNECT_TIMEOUT_ERROR      = 599,
};

static std::unordered_map<Code, std::string> CodeMessage = {
    // Informational
    {Code::CONTINUE, "Continue"},
    {Code::SWITCHING_PROTOCOL, "Switching Protocol"},
    {Code::PROCESSING, "Processing"},

    // Success
    {Code::OK, "OK"},

    // Client Error
    {Code::BAD_REQUEST, "Bad Request"},
    {Code::NOT_FOUND, "Not Found"},

    // ServerError
    {Code::INTERNAL_SERVER_ERROR, "Internal Server Error"},
};

namespace Headers {

static std::unordered_set<std::string> General =  {
    "Cache-Control",
    "Connection",
    "Date",
    "MIME-Version",
    "Pragma",
    "Trailer",
    "Transfer-Encoding",
    "Upgrade",
    "Vid",
    "Warning"
};

static std::unordered_set<std::string> Request = {
    "Accept",
    "Accept-Charset",
    "Accept-Encoding",
    "Accept-Language",
    "Authorization",
    "Content-Disposition",
    "Expect",
    "From",
    "Host",
    "If-Match",
    "If-Modified-Since",
    "If-None-Match",
    "If-Range",
    "If-Unmodified-Since",
    "Max-Forwards",
    "Proxy-Authorization",
    "Range",
    "Referer",
    "TE",
    "User-Agent",

};

static std::unordered_set<std::string> Response = {
    "Accept-Ranges",
    "Age",
    "Alternates",
    "Content-Disposition",
    "ETag",
    "Location",
    "Proxy-Authenticate",
    "Public",
    "Retry-After",
    "Server",
    "Vary",
    "WWW-Authenticate",
};

static std::unordered_set<std::string> Entity = {
    "Allow",
    "Content-Disposition",
    "Content-Encoding",
    "Content-Language",
    "Content-Length",
    "Content-Location",
    "Content-MD5",
    "Content-Range",
    "Content-Type",
    "Content-Version",
    "Derived-From",
    "Expires",
    "Last-Modified",
    "Link",
    "Title",
};

} // namesapce Header

} // namespace Network

#endif  // HTTP_HTTP_COMMON_H_
