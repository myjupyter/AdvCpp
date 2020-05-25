#include <iostream>
#include <utility>
#include <algorithm>
#include <ctime>

#include "file.h"

using Key = std::pair<uint64_t, uint64_t>;

struct Data {
    uint8_t payload[20] = {};
};


constexpr int SIZE = 10;

int main() {
    
    // write indexies
    {
        BinaryFile file("./index", File::out);

        for (int i = 0; i < SIZE; i++) {
            std::string s = std::to_string(i);

            Key ko = std::make_pair(std::hash<std::string>{}(s), sizeof(Data) * i);
            file.write(&ko, sizeof(Key));
        }
    }

    // write data
    {
        BinaryFile file("./data", File::out);

        for (int i = 0; i < SIZE; i++) {
            std::string s = std::to_string(i);

            Data data;
            std::strncpy(reinterpret_cast<char*>(data.payload), s.data(), s.size() > 20 ? 20 : s.size());

            file.write(&data, sizeof(Data));
        }
    }


    IndexFile<uint64_t, uint64_t> file(File{"./index"}, MappedFile::in, MappedFile::shared);
    BinaryFile data_file("./data");


    std::srand(std::time(nullptr));
    for (int i = 0 ; i < 10; i++) {
        int num = std::rand() % SIZE;
        uint64_t key = std::hash<std::string>{}(std::to_string(num));
        
        Data data;
        data_file.readAt(file.table_[key], &data, sizeof(Data));

        std::cout << num << " " << data.payload << std::endl;
    }

    return 0;
}
