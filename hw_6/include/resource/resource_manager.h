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
        static ResourceManager& getInstance() {
            static ResourceManager instance;
            return instance;
        }

        ~ResourceManager() = default;

        void setResourcePath(const std::string resource_path) {
            res_path = resource_path;
        }

        std::string getResource(const std::string& uri, bool caching = false) {
            fs::path uri_path = uri;
            uri_path = uri_path.relative_path();
            fs::path resource = res_path / uri_path;

            if (caching && cache_.contains(resource)) {
                return cache_[resource];
            }

            if (fs::is_regular_file(resource)) {
                std::ifstream ifs(resource.string());
                
                std::string res((std::istreambuf_iterator<char>(ifs)), 
                                std::istreambuf_iterator<char>());

                if (caching) {
                    cache_.insert({resource.string(), std::move(res)});
                 
                    return cache_[resource.string()];
                }

                return res;
            }

            return std::string();
        }

    private:
        ResourceManager() {
            res_path = fs::current_path();
        } 

        fs::path res_path;
        Cache cache_;
};

}  // namespace Network::ResourceManager

#endif  // RESOURCE_RESOURCE_MANAGER_H_
