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
        explicit MyHttp(const Network::IpAddress& address)
            : HttpServer(address, nullptr) {}

        HttpPacket onRequest(const HttpPacket& request) override {
            auto [method, uri] = request.getRequestLine();

            HttpPacket response;

            if (method == Method::GET) {
                std::string req = request.toString();

                ResourceManager& res_man = ResourceManager::getInstance();
                std::string body = res_man.getResource(uri, true);

                if (body.empty()) {
                    response.makeResponse("1.1", Code::NOT_FOUND);
                    response.setBody("404 Not Found");
                } else {
                    response.makeResponse("1.1", Code::OK);
                    response.setBody(std::move(body));
                }
                
            } else if (method == Method::PUT) {
                response.makeResponse("1.1", Code::OK);
                std::string buffer = request.getBody();
                response.setBody(buffer);
            } else if (method == Method::POST) {
                response.makeResponse("1.1", Code::NOT_FOUND);
                response.setBody("This was POST");        
            } else if (method == Method::DELETE) {
                response.makeResponse("1.1", Code::NOT_FOUND);
                response.setBody("This was DELETE");
            }

            info(access_log(request, response));
            return response;
        }
};

#endif  // EXAMPLE_MYHTTP_H_
