#include "myhttp.h"
#include "configurator.h"

using namespace Network;

int main() {

    Logger& log = Logger::getInstance();
    log.setGlobalLogger(create_stderr_logger());

    Config::Configurator& config = Config::Configurator::getInstance();
    config.readConfigsFrom("./config/config.ini");

    set_level(Level::Info);

    try {
        MyHttp server({Config::Configurator::get<std::string>("MyHttp", "ip"),
                       Config::Configurator::get<uint16_t>("MyHttp", "port")});

        server.work(Config::Configurator::get<int>("MyHttp", "threads"),
                    Config::Configurator::get<double>("MyHttp", "timeout"));

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
