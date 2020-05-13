#ifndef RESOURCE_RESOURCE_MANAGER_H_
#define RESOURCE_RESOURCE_MANAGER_H_

#include <string>
#include <filesystem>
#include <fstream>
#include <streambuf>

namespace fs = std::filesystem;

namespace Network::ResourceManager {

class ResourceManager {
    public:    
    ResourceManager() {
        res_path = fs::current_path();
    }
    ResourceManager(const std::string resource_path) {
        res_path = resource_path;
    }
    ~ResourceManager() = default;

    std::string getResource(const std::string& uri) {
        fs::path uri_path = uri;
        uri_path = uri_path.relative_path();
        fs::path resource = res_path / uri_path;

        std::cout << resource.string() << std::endl;        
        if (fs::is_regular_file(resource)) {
            std::ifstream ifs(resource.string());
            return std::string((std::istreambuf_iterator<char>(ifs)),
                               std::istreambuf_iterator<char>());
        }
        return std::string();
    }

    private:
        fs::path res_path;

};

}  // namespace Network::ResourceManager

#endif  // RESOURCE_RESOURCE_MANAGER_H_
