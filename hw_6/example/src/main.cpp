#include "myhttp.h"


int main() {
    Logger& log = Logger::getInstance();
    log.setGlobalLogger(create_stderr_logger());

    set_level(Level::Info);

    try {
        MyHttp server({"0.0.0.0", 8081});

        server.work(4, 2.5);

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
