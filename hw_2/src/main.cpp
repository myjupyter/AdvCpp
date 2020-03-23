#include <global_log_func.h>

#include <thread>
#include <functional>

#define TIME 10000

void example(std::function<void(const std::string&)> func) {
    for (int i = 0; i < TIME; ++i) {
        func(std::to_string(i));
    }
}


int main(int argc, char* argv[]) {
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_stdout_logger());

    std::thread t1(example, std::ref(Log::debug));
    std::thread t2(example, std::ref(Log::warning));

    t1.join();
    t2.join();

    return 0;
}
