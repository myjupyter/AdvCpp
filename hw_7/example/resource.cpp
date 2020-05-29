#include <iostream>
#include <utility>
#include <algorithm>
#include <ctime>

#include "file.h"
#include "generator.h"


constexpr std::size_t COUNT = 10000000;

int main() {
    std::string file("./data/text.dat");

    generateKeyDataFile(file, COUNT);

    DataStorage storage(file);
    if (!storage.isSorted()) {
        storage.sort();
    }
    if (!storage.isSorted()) {
        std::cout << "Still not sorted" << std::endl;
    }
    storage.createIndex("./data/index.dat");


    std::srand(std::time(nullptr));
    for (int i = 0; i < 1000; ++i) {

        std::size_t j = ::rand() % COUNT;
        auto val = storage.search(std::hash<std::string>{}(std::to_string(j)));

        if (val.has_value()) {
            std::size_t res = static_cast<std::size_t>(std::stoi(reinterpret_cast<char*>(val.value().payload)));
            if (j != res) {        
                std::cout << j << " != " << res << std::endl;
            }
        } else {
            std::cout << "Not found: " << j << std::endl;
        }
    }
    return 0;
}
