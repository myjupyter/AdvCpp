#ifndef RESOURCE_RESOURCE_MANAGER_H_
#define RESOURCE_RESOURCE_MANAGER_H_

#include <string>
#include <filesystem>
#include <fstream>
#include <streambuf>
#include <map>

namespace fs = std::filesystem;

namespace Network::ResourceManager {

class ResourceManager {
    public:
        using Cache = std::map<std::string, std::string>;

    public:    
        ~ResourceManager() = default;

        static ResourceManager& getInstance();
        void setResourcePath(const std::string& resource_path);
        std::string getResource(const std::string& uri, bool caching = false);

    private:
        ResourceManager();

        fs::path res_path_;
        Cache cache_;
};

}  // namespace Network::ResourceManager

#endif  // RESOURCE_RESOURCE_MANAGER_H_
