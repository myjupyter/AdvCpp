#include "client_tcp.h"

static const int BUFFER_SIZE = 4097;

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
    if (new_current > data_.size() - 1) {
        return false;
    }

    data = std::move(std::string(data_.begin() + current_pos_, data_.begin() + new_current));
    current_pos_ = new_current + delim.size();
    return true;
}

bool BytePackage::hasData() const {
    return current_pos_ < data_.size() - 1;
}

// Client Tcp

ClientTcp& ClientTcp::operator>>(std::string& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    connection_.read(buffer.data(), buffer.size() - 1);
    if (std::strlen(buffer.data())) {
        std::copy(buffer.begin(), buffer.end(), std::back_inserter(package));
    }
    return *this;
}

ClientTcp& ClientTcp::operator>>(BytePackage& package) {
    std::string buffer(BUFFER_SIZE, '\0');
    connection_.read(buffer.data(), buffer.size() - 1);
    if (std::strlen(buffer.data())) {
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
