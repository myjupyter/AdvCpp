#include <iostream>
#include "file.h"

int main() {

    //ResourceManager manager = ResourceManager::getInstance();
    //std::cout << manager.getResource("example/main.cpp") << std::endl;
    
    MappedFile file(File("example/main.cpp"), MappedFile::in, MappedFile::shared);


    return 0;
}
