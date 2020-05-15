#include "resource_manager.h"

namespace Network::ResourceManager {

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

void ResourceManager::setResourcePath(const std::string& resource_path) {
    res_path_ = resource_path; 
}

std::string ResourceManager::getResource(const std::string& uri, bool caching) {
    fs::path uri_path = uri;
    uri_path = uri_path.relative_path();
    fs::path resource = res_path_ / uri_path;

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

ResourceManager::ResourceManager() {
    res_path_ = fs::current_path();
}

} // namespace Network::ResourceManager
