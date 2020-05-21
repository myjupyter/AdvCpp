#include <iostream>
#include "resource_manager.h"

using namespace Network::ResourceManager;

int main() {

    ResourceManager manager = ResourceManager::getInstance();

    std::cout << manager.getResource("example/main.cpp") << std::endl;
    
    return 0;
}
