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

        std::size_t read(void* data, std::size_t size) override {
            std::size_t bytes = ::fread(data, size, 1, file_ptr_.get());
            if (bytes < 1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "BinaryFile::read");
            }
            return bytes;
        }

        virtual void close() override {
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

            std::cout << offset << " " << ftell(file_ptr_.get()) << std::endl; 

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

        enum Mode {
            in = PROT_READ,
            out = PROT_WRITE
        };

        enum MapMode {
            shared = MAP_SHARED,
            unique = MAP_PRIVATE
        };
    
    public:        
        MappedFile(File&& file, Mode m = Mode::in, MapMode mm = MapMode::shared)
            : file_(std::move(file)) {

            std::size_t size = file_.getSize();
            void* ptr = ::mmap(0, size, static_cast<int>(m), 
                               static_cast<int>(mm), file_.fd_, 0);
            if (ptr == MAP_FAILED) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "MappedFile::MappedFile");
            }

            mapped_file_ = std::move(MappedFilePtr{reinterpret_cast<char*>(ptr), [size] (char* ptr) {
                if (ptr != nullptr) {
                    ::munmap(ptr, size);
                }    
            }});
        }

        void close() {
            mapped_file_.reset(nullptr);
            file_.close();
        }

        ~MappedFile() = default;

    protected:
        MappedFile() = delete;

        File file_;
        MappedFilePtr mapped_file_ = nullptr;
};


template <typename Key, typename T>
class IndexFile : public MappedFile {
    public:
        using value_type = std::pair<Key, T>;

    public:
        IndexFile(File&& file, Mode m = Mode::in, MapMode mm = MapMode::shared)
            : MappedFile(std::move(file), m, mm) {
            void* ptr = mapped_file_.get();
            std::size_t size = file_.getSize();
            
            for (uint64_t i = 0; i < size; i += sizeof(value_type)) {
                table_.emplace(*(reinterpret_cast<value_type*>(reinterpret_cast<char*>(ptr) + i)));        
            }

            close();
        }

    public:
        std::unordered_map<Key, T> table_;
};

#endif  // SYSTEM_FILE_H_
