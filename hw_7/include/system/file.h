#ifndef SYSTEM_FILE_H_
#define SYSTEM_FILE_H_

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <unistd.h>

#include <memory>
#include <string>
#include <stdexcept>
#include <functional>
#include <unordered_map>

#include <iostream>

class MappedFile;

class File {
    public:
        enum Flags {
            in  = O_RDONLY,
            out = O_WRONLY | O_CREAT | O_TRUNC,
        };

    public:
        explicit File(const std::string& path, Flags flag = Flags::in) {
            fd_ = ::open(path.c_str(), static_cast<int>(flag));
            if (fd_ == -1 || fstat(fd_, &info_) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::File");
            }
        }

        File(File&& file)
            : fd_(file.fd_)
            , info_(file.info_) {
            file.fd_ = -1;
            ::memset(&file.info_, '\0', sizeof(struct stat));
        }

        File& operator=(File&& file) {
            if (this != &file) {
                fd_ = file.fd_;
                ::memcpy(&info_, &file.info_, sizeof(struct stat));

                file.fd_ = -1;
                ::memset(&file.info_, '\0', sizeof(struct stat));
            }
            return *this;
        }

        std::size_t getSize() const {
            return info_.st_size;
        }

        virtual void close() {
             if (fd_ != -1) {
                ::close(fd_);
                
                fd_ = -1;
                ::memset(&info_, '\0', sizeof(struct stat)); 
             }
        }

        virtual ~File() {
            close();
        }

        virtual std::size_t write(const void* data, std::size_t size) {
            ssize_t bytes = ::write(fd_, data, size);
            if (bytes == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::write");
            }
            return bytes;
        }

        virtual std::size_t read(void* data, std::size_t size) {
            ssize_t bytes = ::read(fd_, data, size);
            if (bytes == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::write");
            }
            if (bytes == 0) {
                close();
            }
            return bytes;
        }

    protected: 
        File() : fd_{-1} {}
        
        int fd_;
        struct stat info_;

        friend MappedFile;
};

class BinaryFile : public File {
    public:
        using FILEPtr = std::unique_ptr<FILE>;
    
    public:
        explicit BinaryFile(const std::string& path, Flags flag = Flags::in)
            : File(path, flag) {
            const char* mode = (flag == Flags::in) ? "rb" : "wb";
            
            FILE* ptr = fdopen(fd_, mode);
            if (ptr == NULL) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "BinaryFile::BinaryFile");
            }
            file_ptr_ = FILEPtr(ptr);
        }

        BinaryFile(BinaryFile&& file) = default;
        BinaryFile& operator=(BinaryFile&& file) = default;

        std::size_t write(const void* data, std::size_t size) override {
            std::size_t bytes = ::fwrite(data, size, 1, file_ptr_.get());
            if (ferror(file_ptr_.get())) {
                clearerr(file_ptr_.get());
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                       "BinaryFile::write");
            }
            return bytes;
        }

        std::size_t read(void* data, std::size_t size) override {
            std::size_t bytes = ::fread(data, size, 1, file_ptr_.get());
            if (ferror(file_ptr_.get())) {
                clearerr(file_ptr_.get());
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "BinaryFile::read");
            }
            if (feof(file_ptr_.get())) {
                clearerr(file_ptr_.get());
                return 0;
            }
            return bytes;
        }

        void close() override {
            if (file_ptr_ != nullptr) {
                fclose(file_ptr_.release());
            }
        }

        ~BinaryFile() {
            close();
        } 

        std::size_t writeAt(long offset, const void* buf, std::size_t size) {
            if (fseek(file_ptr_.get(), offset, SEEK_SET) < 0) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "BinaryFile::readAt");
            }
        
            std::size_t bytes = write(buf, size);
            rewind(file_ptr_.get());

            return bytes;
        }

        std::size_t readAt(long offset, void* buf, std::size_t size) {
            if (fseek(file_ptr_.get(), offset, SEEK_SET) < 0) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "BinaryFile::readAt");
            }

            std::size_t bytes = read(buf, size);
            rewind(file_ptr_.get());
            
            return bytes;
        }

    private:
        FILEPtr file_ptr_;
};

class MappedFile {
    public:
        using MappedFilePtr = std::unique_ptr<char, std::function<void(char*)>>;
        using FilePtr       = std::unique_ptr<File>;
    
    public:        
        MappedFile(File&& file, int prot_mode = PROT_READ, int map_mode = MAP_PRIVATE,
                                std::size_t size = 0, std::size_t offset = 0)
            : file_(std::move(file))
            , info_{prot_mode, map_mode, size, offset} {

            mapped_file_ = std::move(MappedFilePtr{reinterpret_cast<char*>(makeMap()), [size] (char* ptr) {
                if (ptr != nullptr) {
                    ::munmap(ptr, size);
                }    
            }});
        }

        void remap(std::size_t new_offset) {
            if (new_offset + info_.block_size > sizeFile()) {
                return;
            }

            info_.block_offset = new_offset;
            mapped_file_.reset(reinterpret_cast<char*>(makeMap()));
        }

        template <typename pointer_type>
        pointer_type* get() const {
            return reinterpret_cast<pointer_type*>(mapped_file_.get());
        }

        std::size_t sizeFile() const {
            return file_.getSize();
        }

        std::size_t sizeMap() const {
            return info_.block_size;
        }

        void close() {
            mapped_file_.reset(nullptr);
            file_.close();
        }

        ~MappedFile() = default;

    private:
        void* makeMap() {
            auto [prot_mode, map_mode, size, offset] = info_;
            
            void* ptr = ::mmap(mapped_file_.release(), size, prot_mode, map_mode, file_.fd_, offset);
            if (ptr == MAP_FAILED) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "MappedFile::makeMap");
            }

            return ptr;
        }

        struct MMapInfo {
            int prot_mode    = PROT_READ;
            int map_mode     = MAP_PRIVATE;
            std::size_t block_size   = 0;
            std::size_t block_offset = 0; 
        };

        MappedFile() = delete;

        File file_;

        MappedFilePtr mapped_file_ = nullptr;
        MMapInfo info_;
};

// read-only
template <typename Key, typename T>
class HashTableIndex {
    public:
        using value_type   = std::pair<Key, T>;
        using pointer_type = value_type*; 

    public:
        HashTableIndex(File&& file, int prot_mode = PROT_READ, int map_mode = MAP_PRIVATE) {
            std::size_t size = file.getSize();

            if (size % sizeof(value_type)) {
                throw std::invalid_argument("File not aligned");
            }
            std::size_t elem_count = size / sizeof(value_type);
            std::size_t block_elem = 100;
            std::size_t block_size = size < (sizeof(value_type) * block_elem) ? size : (sizeof(value_type) * block_elem);

            MappedFile mapped_file(std::move(file), prot_mode, map_mode, block_size, 0);
             
            std::size_t left_to_read = 0;
            for (std::size_t i = 0; i < elem_count; i+= left_to_read) {
                
                left_to_read = (elem_count - i) < block_elem ? (elem_count - i) : block_elem;

                for (std::size_t j = 0; j < left_to_read; j++) {
                    table_.emplace(*(mapped_file.get<value_type>() + j)); 
                }

                mapped_file.remap((i + left_to_read) * sizeof(value_type));
            }
        }

        T operator[](const Key& key) {
            return table_[key];
        }

        T operator[](Key&& key) {
            return table_[std::move(key)];
        }

        bool contains(const Key& key) const {
            return table_.contains(key);
        }

    public:
        std::unordered_map<Key, T> table_;
};

// read-only
template <typename Key, typename T>
class LargeData {

    public:
        LargeData(const std::string& index_f, const std::string& data_f)
            : index_{std::move(File(index_f))}
            , data_{data_f} {}

        LargeData(File&& index_f, BinaryFile data_f) 
            : index_{std::move(index_f)}
            , data_{std::move(data_f)} {}

        T operator[](const Key& key) {
            T data;
            if (!index_.contains(key)) {
                return data;
            }

            uint64_t offset = index_[key]; 
            data_.readAt(offset, &data, sizeof(T)); 
            
            return data;
        }

    private:
        HashTableIndex<Key, uint64_t> index_;
        BinaryFile data_;
};

#endif  // SYSTEM_FILE_H_
