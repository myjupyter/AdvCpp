#include "client_tcp.h"

#include <iostream>

#include "client_tcp_ecxep.h"

static const int BUFFER_SIZE = 1 << 16;

namespace Network {

// BytePackage

BytePackage& BytePackage::operator<<(const std::string& data) {
    std::copy(data.begin(), data.begin() + std::strlen(data.data()), std::back_inserter(data_));            
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

std::size_t BytePackage::fullSize() const {
    return data_.size();
}

std::size_t BytePackage::size() const {
    return data_.size() - current_pos_;
}

const char* BytePackage::fullData() const {
    return data_.data();
}

const char* BytePackage::data() const {
    return data_.data() + current_pos_;
}

std::string BytePackage::getNBytes(std::size_t n) {
    auto real_size = size() < n ? size() : n;
    std::string data(data_.begin() + current_pos_, data_.begin() + current_pos_ + real_size);
    current_pos_ += real_size;
    return data;
}

std::string BytePackage::toString() const {
    return std::string(data_.begin() + current_pos_, data_.end());
}

void BytePackage::clear() {
    current_pos_ = 0;
    data_.clear();
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
        throw Exceptions::ClientDisconnect("Client Disconnected");
    }
    return *this;
}

ClientTcp& ClientTcp::operator>>(BytePackage& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    std::size_t bytes = 0;
    while ((bytes = connection_.read(buffer.data(), buffer.size() - 1)) != 0) {
        package << buffer;
    }
    if (bytes == 0) {
        throw Exceptions::ClientDisconnect("Client Disconnected");
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

ssize_t ClientTcp::async_read(std::string& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    ssize_t bytes = 0;
    
    if ((bytes = connection_.read_non_block(buffer.data(), buffer.size() - 1)) == 0) {
        throw Exceptions::ClientDisconnect("Client Disconnected");
    } else if (bytes != -1) {
        std::copy(buffer.begin(), buffer.begin() + bytes, std::back_inserter(package));
    }
    return bytes;
}

ssize_t ClientTcp::async_read(BytePackage& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    ssize_t bytes = 0;

    if ((bytes = connection_.read_non_block(buffer.data(), buffer.size() - 1)) == 0) {
        throw Exceptions::ClientDisconnect("Client Disconnected");
    } else if (bytes != -1) {
        package << buffer;
    }
    return bytes;
}

ssize_t ClientTcp::async_write(BytePackage& package) {
    ssize_t bytes = connection_.write_non_block(package.data(), package.size());
    if (bytes != -1) {
        package.current_pos_ += bytes;
    }
    return bytes;
}

ssize_t ClientTcp::async_write(const void* data, std::size_t size) {
    return connection_.write_non_block(data, size);
}

}  // namespace Network
