#include <global_log_func.h>

#include <thread>
#include <functional>
#include <iostream>

#include <unistd.h>

const int TIME =  10;

void example(std::function<void(const std::string&)> func) {
    for (int i = 0; i < TIME; ++i) {
        sleep(0.2);
        func(std::to_string(i));
    }
}

int main(int argc, char* argv[]) {
    
    Log::Logger& log = Log::Logger::getInstance();
    log.setGlobalLogger(Log::create_file_logger("logging"));

    std::thread t1(example, std::ref(Log::debug));
    std::thread t2(example, std::ref(Log::warning));
    std::thread t3(example, std::ref(Log::info));
    std::thread t4(example, std::ref(Log::error));

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    return 0;
}
