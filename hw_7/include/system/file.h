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
#include <mutex>

#include "file_cont.h"
#include "non_copyable.h"

class File : Network::NonCopyable {
    public:
        explicit File(const std::string& path, int flag, int mode) {
            fd_ = ::open(path.c_str(), flag, mode);
            if (fd_ == -1 || fstat(fd_, &info_) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::File");
            }
            name_ = path;
        }

        File(File&& file)
            : fd_(file.fd_)
            , name_(file.name_)
            , info_(file.info_) {
            file.fd_ = -1;
            file.info_ = {};
        }

        File& operator=(File&& file) {
            if (this != &file) {
                fd_ = file.fd_;
                name_ = std::move(file.name_);
                file.info_ = info_;

                file.fd_ = -1;
                file.info_ = {};
            }
            return *this;
        }

        std::size_t getSize() {
            if (fstat(fd_, &info_) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::getSize");
            }
            return info_.st_size;
        }

        std::size_t getFd() const {
            return fd_;
        }

        
        void setFileSize(std::size_t bytes) {
            if(::ftruncate(fd_, bytes) == -1) {
                throw std::runtime_error(std::strerror(errno));
            }
        }

        void deleteFile() {
            if (fd_ != -1) {
                close();
                ::remove(name_.c_str());
            }
        }

        void renameFile(const std::string& name) {
            if (fd_ != -1) {
                if (::rename(name_.c_str(), name.c_str()) == -1) {
                    throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                            "File::renameFile");
                }
            }
        } 

        std::string getName() const {
            return name_;
        }

        virtual void close() {
             if (fd_ != -1) {
                ::close(fd_);
                
                fd_ = -1;
                info_ = {};
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
                                        "File::read");
            }
            return bytes;
        }

        File() : fd_{-1} {}
    
    private:     
        int fd_;
        std::string name_;
        struct stat info_;
};


class DataStorage {
    public:
        explicit DataStorage(const std::string& path)
            : data_(path, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
            , name_(path) {
                std::size_t elem_count = data_.getSize() / sizeof(Pair);
                FVector vec(elem_count, Allocator<Pair>(data_.getFd()));
                vec_data_ = std::move(vec);
            }

        void sort(std::size_t chunk_size = ::sysconf(_SC_PAGESIZE) * 100000 / 4) {
            std::vector<FVector> fvectors;
           
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

            std::size_t elem_count = data_.getSize() / sizeof(Pair);
            std::size_t elem_per_page = chunk_size / sizeof(Pair);
            std::size_t offsets = elem_count;
            
            // Chunk sort
            for (int offset = 0; offset <= offsets; offset+= elem_per_page) {
                std::size_t elem = elem_count - offset < elem_per_page ? elem_count - offset : elem_per_page;
                if (elem == 0) {
                    break;
                }

                FVector vec(elem, Allocator<Pair>(data_.getFd(), offset * sizeof(Pair)));
                std::sort(vec.begin(), vec.end(), [](const Pair x, const Pair& y) -> bool {
                        return x.key < y.key;
                });

                fvectors.push_back(std::move(vec));
            }

            std::vector<std::pair<Iterator, Iterator>> iterators;

            std::for_each(fvectors.begin(), fvectors.end(), [&iterators] (auto& vec){
                    iterators.push_back({vec.begin(), vec.end()});
            });

            // Merge Sort
            File result(name_ + ".bk", O_CREAT | O_TRUNC | O_RDWR, mode);
            result.setFileSize(elem_count * sizeof(Pair));
            FVector vec(elem_count,  Allocator<Pair>(result.getFd()));

            for (std::size_t i = 0; i < elem_count; ++i) {
                auto it_to_it = std::min_element(iterators.begin(), iterators.end(), [](const auto& pair_x, const auto& pair_y) -> bool {
                        return pair_x.first->key < pair_y.first->key;
                });
                Iterator& it = it_to_it->first;
                vec[i] = *(it++);

                if (it == it_to_it->second) {
                    iterators.erase(it_to_it);
                }

            }

            vec_data_ = std::move(vec);
            data_.deleteFile();
            data_ = std::move(result);
            data_.renameFile(name_);
        }

        void createIndex(const std::string& index_path) {
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

            std::size_t elem_count = data_.getSize() / sizeof(Pair);
            std::size_t pair_per_page = ::sysconf(_SC_PAGESIZE) / sizeof(Pair);
            std::size_t index_count = elem_count / pair_per_page + (elem_count % pair_per_page ? 1 : 0);
            
            FVector vec(elem_count, Allocator<Pair>(data_.getFd()));

            File index(index_path, O_CREAT | O_TRUNC | O_RDWR, mode);
            index.setFileSize(index_count * sizeof(Index));

            IVector vec_i(index_count,  Allocator<Index>(index.getFd()));
            for (std::size_t i = 0; i < index_count; ++i) {
                vec_i[i].key    = vec[i * pair_per_page].key;
                vec_i[i].offset = i * pair_per_page * sizeof(Pair);
            }

            index_ = std::move(vec_i);
        }

        bool isSorted() {
            return std::is_sorted(vec_data_.begin(), vec_data_.end(), [] (const Pair& x, const Pair& y) -> bool {
                return x.key < y.key;        
            });
        }

        void setIndex(const std::string& index_path) {
            mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
            
            std::size_t elem_count = data_.getSize() / sizeof(Pair);
            std::size_t pair_per_page = ::sysconf(_SC_PAGESIZE) / sizeof(Pair);
            std::size_t index_count = elem_count / pair_per_page + (elem_count % pair_per_page ? 1 : 0);
            
            File index(index_path, O_RDWR, mode);
            IVector vec_i(index_count,  Allocator<Index>(index.getFd()));
        
            index_      = std::move(vec_i);
            index_file_ = std::move(index);
        }

        std::optional<Data> search(uint64_t hash_key) {
            Index ind;
            ind.key = hash_key;
           

            auto it = std::upper_bound(index_.begin(), index_.end(), ind, [](const Index& x, const Index& y) -> bool {
                return x.key < y.key;
            });

            if (it == std::end(index_) || it == std::begin(index_)) {
                if (it == std::begin(index_)) {
                    return {};
                }
                it == index_.end();
            }

            std::size_t elem_count = data_.getSize() / sizeof(Pair);


            std::size_t pair_per_page = ::sysconf(_SC_PAGESIZE) / sizeof(Pair);
            std::size_t offset = (--it)->offset;
            std::size_t rest =  elem_count - (offset / sizeof(Pair));
            std::size_t elem_offset = offset / sizeof(Pair);
            std::size_t elem = rest < pair_per_page ? rest : pair_per_page;

            Pair p;
            p.key = hash_key;
            auto vec_it = vec_data_.begin() + elem_offset;

            auto elem_it = std::lower_bound(vec_it, vec_it + elem, p, [](const Pair& x, const Pair& y) -> bool {   
                    return x.key < y.key;
            });

            if (elem_it == std::end(vec_data_) || elem_it->key != hash_key) { 
                return {};
            }

            return elem_it->data;
        }

    private:
        File index_file_;
        IVector index_;

        std::string name_;

        File data_;
        FVector vec_data_;
};

#endif  // SYSTEM_FILE_H_
