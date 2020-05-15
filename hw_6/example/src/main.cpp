#include "myhttp.h"


int main() {
    Logger& log = Logger::getInstance();
    log.setGlobalLogger(create_stderr_logger());

    try {
        MyHttp server({"127.0.0.1", 8080});

        server.work(4, 2.5);

    } catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    return 0;
}
