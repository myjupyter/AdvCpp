#include "myhttp.h"
#include "configurator.h"
#include "file.h"


using namespace Network;

int main() {

    Logger& log = Logger::getInstance();
    log.setGlobalLogger(create_stderr_logger());

    Config::Configurator& config = Config::Configurator::getInstance();
    config.readConfigsFrom("./config/config.ini");

    set_level(Level::Info);


    DataStorage storage(Config::Configurator::get<std::string>("Storage", "data"));
    if (!storage.isSorted()) {
        storage.sort();
    }
    storage.createIndex(Config::Configurator::get<std::string>("Storage", "index"));

    try {
        MyHttp server({Config::Configurator::get<std::string>("MyHttp", "ip"),
                       Config::Configurator::get<uint16_t>("MyHttp", "port")});


        server.HttpHandler("GET", [&storage](const HttpPacket& req) -> HttpPacket {
            auto [method, uri] = req.getRequestLine();
            
            HttpPacket response;            
        
            uri.erase(std::remove(uri.begin(), uri.end(), '/'), uri.end());

            auto val = storage.search(std::hash<std::string>{}(uri));
            if (val.has_value()) {
                response.makeResponse("1.1", Code::OK);
                std::string str(reinterpret_cast<char*>(val.value().payload));
                response.setBody(str);
            } else {
                response.makeResponse("1.1", Code::NOT_FOUND);
                response.setBody("404 Not Found");
            }
            return response;         
        });

        server.work(Config::Configurator::get<int>("MyHttp", "threads"),
                    Config::Configurator::get<double>("MyHttp", "timeout"));

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
