#ifndef CONFIG_CONFIGURATOR_H_
#define CONFIG_CONFIGURATOR_H_

#include <fstream>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <regex>
#include <iostream>

const std::regex regex_key_value("( )*(.+?)( )*=( )*(.+?)+( )*");
const std::regex regex_section("\\[(\\w)+\\]");

namespace Network::Config {

class ConfiguratorExcep : public std::exception {
    public:
        ConfiguratorExcep(const std::string& err_msg)
            : msg_(err_msg) {}

        virtual const char* what() const noexcept {
            return msg_.c_str();
        }

        ~ConfiguratorExcep() = default;

    private:
        std::string msg_;
};

class ConfiguratorBadFile : public ConfiguratorExcep {
    public:
        ConfiguratorBadFile(const std::string& err_msg)
           : ConfiguratorExcep(err_msg) {}
};

class ConfiguratorBadContent : public ConfiguratorExcep { 
    public:
        ConfiguratorBadContent(const std::string& err_msg)
           : ConfiguratorExcep(err_msg) {}
};

class ConfiguratorFieldNotExist : public ConfiguratorExcep { 
    public:
        ConfiguratorFieldNotExist(const std::string& err_msg)
           : ConfiguratorExcep(err_msg) {}
};

class Configurator {
    public:
        using Section  = std::unordered_map<std::string, std::string>;
        using Sections = std::unordered_map<std::string, Section>;
   
    public:
        static Configurator& getInstance() {
            static Configurator instance;
            return instance;
        }

        void readConfigsFrom(const std::string& path) {
            configs_.clear();
            if (config_file_.is_open()) {
                config_file_.close();
            }

            config_file_.open(path);
            if (!config_file_.is_open()) {
                throw ConfiguratorBadFile("Can't open file");
            }

            std::string current_section;
            for (std::string line; std::getline(config_file_, line, '\n');) {
                if (line.empty()) {
                    continue;
                }

                std::size_t pos = line.find_first_not_of(" ");
                std::size_t pos2 = line.find_last_not_of(" ");
                line = std::string(line.begin() + pos, line.begin() + pos2 + 1);
    
                pos = line.find('#');
                pos2 = line.find(';');
                pos = pos < pos2 ? pos : pos2;
                if (pos != std::string::npos) {
                    line = std::string(line.begin(), line.begin() + pos);
                }

                if (line.empty()) {
                    continue;
                }

                if (std::regex_search(line, regex_section)) {
                    std::size_t o = line.find("[");
                    std::size_t c = line.find("]");

                    current_section = std::string(line.begin() + o + 1, line.begin() + c);
                    configs_.insert({current_section, Section()});
                } else if (std::regex_search(line, regex_key_value)) {
                    std::size_t eq_pos = line.find("=");
                    std::string key = std::string(line.begin(), line.begin() + eq_pos++);
                    key.erase(std::remove(key.begin(), key.end(), ' '), key.end());

                    std::string value = std::string(line.begin() + eq_pos, line.end());
                    value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
                
                    configs_[current_section].insert({key, value});
                } else {
                    throw ConfiguratorBadContent("File " + path + " has unrecognized config fields");
                }
            }

            config_file_.close();
        }

        template <typename T>
        static auto get(const std::string& section, const std::string& field) -> T {
            Configurator& config = getInstance(); 

            if (!config[section].contains(field)) {
                throw ConfiguratorFieldNotExist("Section " + section + " has no field " + field);
            }
            std::string value = config[section][field];

            if constexpr (std::is_integral<T>::value) {
                return static_cast<T>(std::stoull(value));
            }
            if constexpr (std::is_floating_point<T>::value ) {
                return static_cast<T>(std::stold(value));
            } 
            if constexpr (std::is_same<T, std::string>::value) {
                return value;
            } else {
                throw ConfiguratorBadContent("Bad type conversion");
            }
        }

        Section& operator[](const std::string& section) {
            return configs_[section];
        }


    private:
        Configurator() = default;
        
        Sections configs_;
        std::ifstream config_file_;
};

}  // namespace Network::Config

#endif  // CONFIG_CONFIGURATOR_H_
