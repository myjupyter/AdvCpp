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

class Descriptor : Network::NonCopyable {
    public:
        Descriptor() : desc_{-1} {}

        explicit Descriptor(int desc) {
            if (desc == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "Descriptor::Descriptor");
            }
            desc_ = desc;
        } 

        Descriptor(Descriptor&& desc) 
            : desc_(desc.desc_) {
            desc.desc_ = -1;
        }

        Descriptor& operator=(Descriptor&& desc) {
            if (this != std::addressof(desc)) {
                desc_ = desc.desc_;

                desc.desc_ = -1;
            }
            return *this;
        }

        int getDesc() const {
            return desc_;
        }

        virtual void close() {
            if (desc_ != -1) {
                ::close(desc_);

                desc_ = -1;
            }
        }

        virtual ~Descriptor() {
            close();
        }


    protected:
        int desc_;
};

class FileStat {
    public:
        FileStat() : info_{} {}

        explicit FileStat(int fd) {
            updateInfo(fd);
        }

        FileStat(const FileStat&) = default;
        FileStat(FileStat&&) = default;
        FileStat& operator=(const FileStat&) = default;
        FileStat& operator=(FileStat&&) = default;
        ~FileStat() = default;

        std::size_t getSize(int fd) {
            updateInfo(fd);
            return info_.st_size;
        }

    private:
        void updateInfo(int fd) {
            if (::fstat(fd, &info_) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "FileStat::FileStat");
            }
        }

        struct stat info_;
};

class File : public Descriptor {
    public: 

        explicit File(const std::string& name, int flag, int mode)
            : Descriptor(::open(name.c_str(), flag, mode))
            , name_(name)
            , info_() {}

        File() = default;
        File(File&&) = default;
        File& operator=(File&&) = default;
        ~File() = default;

        void setFileSize(std::size_t bytes) {
            if(::ftruncate(desc_, bytes) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::setFileSize");
            }
        }

        void deleteFile() {
            if (desc_ != -1) {
                close();
                ::remove(name_.c_str());
            }
        }

        void renameFile(const std::string& name) {
            if (::rename(name_.c_str(), name.c_str()) == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::renameFile");
            }
        } 

        int getFd() const {
            return getDesc();
        }
        
        std::string getName() const {
            return name_;
        }

        std::size_t getSize() {
            return info_.getSize(desc_);
        }

        std::size_t write(const void* data, std::size_t size) {
            ssize_t bytes = ::write(desc_, data, size);
            if (bytes == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::write");
            }
            return bytes;
        }

        std::size_t read(void* data, std::size_t size) {
            ssize_t bytes = ::read(desc_, data, size);
            if (bytes == -1) {
                throw std::system_error(std::make_error_code(static_cast<std::errc>(errno)),
                                        "File::read");
            }
            return bytes;
        }
       

    private:
        std::string name_;
        FileStat info_;
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
