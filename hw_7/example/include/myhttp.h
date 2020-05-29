#ifndef EXAMPLE_MYHTTP_H_
#define EXAMPLE_MYHTTP_H_

#include "http_server.h"
#include "global_log_func.h"
#include "resource_manager.h"

using namespace Network::Log;
using namespace Network::Http;
using namespace Network::Services;
using namespace Network::ResourceManager;

const char* ACCESS_TIME_FORMAT = "[%d/%b/%Y:%H:%M:%S %z]"; 

static std::string access_log(const HttpPacket& request, const HttpPacket& response) {
    auto [method, uri] = request.getRequestLine();
    auto code          = response.getResponseLine();

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::string len = std::to_string(response.getContentLength());
    std::stringstream log_message;
    log_message << request.getField("X-Forwarded-For")
                << " -- "
                << std::put_time(&tm, ACCESS_TIME_FORMAT) 
                << " "
                << "\"" +  method + " " + request.getField("Host") + uri + " HTTP/" + request.getVersion() + "\""
                << " " + std::to_string(static_cast<int>(code)) + " " + len 
                << " -- "
                << request.getField("User-Agent");

    return std::string(log_message.str());
}


class MyHttp : public HttpServer {
    public:
        using MethodHandler = std::function<HttpPacket(const HttpPacket& packet)>;
        using HandlerMap    = std::unordered_map<std::string_view, MethodHandler>;

    public:
        explicit MyHttp(const Network::IpAddress& address)
            : HttpServer(address, nullptr) {}

        HttpPacket onRequest(const HttpPacket& request) override {
            auto [method, uri] = request.getRequestLine();

            HttpPacket response;

            if (handler_map_.contains(method)) {
                response = handler_map_[method](request);
            } else {
                response.makeResponse("1.1", Code::NOT_FOUND);
                response.setBody("404 Not Found"); 
            }

            info(access_log(request, response));
            return response;
        }

        void HttpHandler(std::string_view method, MethodHandler handler) {
            handler_map_[method] = handler;
        }

    private:
        HandlerMap handler_map_;        
};

#endif  // EXAMPLE_MYHTTP_H_
