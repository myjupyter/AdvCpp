#ifndef SYSTEM_GENERATOR_H_
#define SYSTEM_GENERATOR_H_

#include "file.h"

void generateKeyDataFile(const std::string& path, std::size_t elem) {
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    File file(path, O_CREAT | O_TRUNC | O_RDWR, mode);

    file.setFileSize(elem * sizeof(Pair));
    FVector vec(elem,  Allocator<Pair>(file.getFd()));

    for (std::size_t i = 0; i < elem; ++i) {
        auto str = std::to_string(i);

        Pair p;
        p.key = std::hash<std::string>{}(str);
        ::memset(&p.data, '\0', sizeof(Data));
        std::strncpy(reinterpret_cast<char*>(p.data.payload), str.data(), 
                                            str.size() > sizeof(Data) ? sizeof(Data) : str.size());
        
        vec[i] = p;
    }
}

#endif // SYSTEM_GENERATOR_H_
