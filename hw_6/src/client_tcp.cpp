#include "client_tcp.h"
#include <iostream>

static const int BUFFER_SIZE = 1 << 16;

namespace Network {

// BytePackage

BytePackage& BytePackage::operator<<(const std::string& data) {
    std::copy(data.begin(), data.end(), std::back_inserter(data_));            
    return *this;
}

BytePackage& BytePackage::operator>>(std::string& data) {
    if (hasData()) {
        data = std::move(std::string(data_.begin() + current_pos_, data_.end()));
        current_pos_ = data_.size() - 1;
    }
    return *this;
}

bool BytePackage::getline(std::string& data, const std::string& delim) {
    if (!hasData()) {
        return false;
    }

    std::size_t new_current = data_.find(delim, current_pos_);
    if (!data_.empty() && new_current > data_.size() - 1) {
        return false;
    }

#ifdef DEBUG
    std::clog <<"Current: " << current_pos_ << " Delimeter at: "<< new_current << " Data Size: " << data_.size() << std::endl;
    #endif

    data = std::move(std::string(data_.begin() + current_pos_, data_.begin() + new_current));
    current_pos_ = new_current + delim.size();
    return true;
}

bool BytePackage::getline(std::string& data, const std::string& delim,
                          std::size_t chunk_size) {
    if (!hasData()) {
        return false;
    }

    std::size_t new_current = data_.find(delim, current_pos_);
    if (!data_.empty() && new_current > data_.size() - 1) {
        return false;
    }

    if (new_current - current_pos_ > chunk_size) {
        data = std::move(std::string(data_.begin() + current_pos_, data_.begin() + current_pos_ + chunk_size));
        current_pos_ += chunk_size;
    } else {
        data = std::move(std::string(data_.begin() + current_pos_, data_.begin() + new_current));
        current_pos_ = new_current + delim.size();
    }
    return true;
}

bool BytePackage::hasData() const {
    return current_pos_ < data_.size() - 1;
}

// Client Tcp

ClientTcp& ClientTcp::operator>>(std::string& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    std::size_t bytes = 0;
    while ((bytes = connection_.read(buffer.data(), buffer.size() - 1)) != 0) {
        std::copy(buffer.begin(), buffer.begin() + bytes, std::back_inserter(package));
    }
    if (bytes == 0) {
        throw std::runtime_error("Client disconnected");
    }
    return *this;
}

ClientTcp& ClientTcp::operator>>(BytePackage& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    while (connection_.read(buffer.data(), buffer.size() - 1)) {
        package << buffer;
    }
    return *this;
}

ClientTcp& ClientTcp::operator<<(std::string& package) {
    connection_.writeExact(package.data(), package.size());
    return *this;
}

ClientTcp& ClientTcp::operator<<(BytePackage& package) {
    std::string buffer;
    package >> buffer;
    return *this << buffer;
}

const ConnectionTcp& ClientTcp::getCon() const {
    return connection_;
}

ConnectionTcp& ClientTcp::getCon() {
    return connection_;
}

}  // namespace Network
