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

class MappedFile;

class File {
    public:
        enum flags {
            in  = O_RDONLY,
            out = O_WRONLY,
        };

    public:
        explicit File(const std::string& path, flags flag = File::in) {
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


        ~File() {
            if (fd_ != -1) {
                ::close(fd_);
                
                fd_ = -1;
                ::memset(&info_, '\0', sizeof(struct stat));
            }
        }

    private: 
        File() : fd_{-1} {}
        
        int fd_;
        struct stat info_;

        friend MappedFile;
};

class MappedFile {
    public:
        using MappedFilePtr = std::unique_ptr<char, std::function<void(char*)>>;
        
        enum Mode {
            in = PROT_READ,
            out = PROT_WRITE
        };

        enum MapMode {
            shared = MAP_SHARED,
            unique = MAP_PRIVATE
        };
    
    public:
        
        MappedFile(File&& file, Mode m, MapMode mm)
            : file_(std::move(file)) {

            std::size_t size = file_.info_.st_size;
            void* ptr = ::mmap(0, size, static_cast<int>(m), 
                               static_cast<int>(mm), file_.fd_, 0);
            if (ptr == MAP_FAILED) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "MappedFile::MappedFile");
            }

            mapped_file_ = std::move(MappedFilePtr{reinterpret_cast<char*>(ptr), [size] (char* ptr) {
                ::munmap(ptr, size);    
            }});
        }

        ~MappedFile() {
            
        }

    private:
        MappedFile() = delete;

        File file_;
        MappedFilePtr mapped_file_ = nullptr;
};

#endif  // SYSTEM_FILE_H_
