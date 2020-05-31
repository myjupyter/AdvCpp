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

constexpr mode_t DEFAULT_FILE_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

class File : public Descriptor {
    public: 

        explicit File(const std::string& name, int flag, int mode = DEFAULT_FILE_MODE)
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

class DataStorage : Network::NonCopyable {
    public:

        DataStorage() = delete;
        ~DataStorage() = default;

        explicit DataStorage(const std::string& path)
            : data_(path, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
            , name_(path) {
                elem_count_ = data_.getSize() / sizeof(Pair);
                elem_per_page_ = ::sysconf(_SC_PAGESIZE) / sizeof(Pair);

                FVector vec(elem_count_, Allocator<Pair>(data_.getFd()));
                vec_data_ = std::move(vec);
            }

        void sort(std::size_t chunk_size = ::sysconf(_SC_PAGESIZE) * 100000 / 4) {
            std::size_t elem_per_chunk = chunk_size / sizeof(Pair);
            
            // Chunk sort
            std::vector<std::pair<Iterator, Iterator>> iterators;
            for (std::size_t elem = 0; elem <= elem_count_; elem+= elem_per_chunk) {
                std::size_t rest = elem_count_ - elem < elem_per_chunk ? elem_count_ - elem : elem_per_chunk;
                if (rest == 0) {
                    break;
                }

                auto chunk_begin = vec_data_.begin() + elem;
                auto chunk_end = chunk_begin + rest;

                std::sort(chunk_begin, chunk_end, [](const Pair x, const Pair& y) {
                        return x.key < y.key;
                });

                iterators.emplace_back(chunk_begin, chunk_end);
            }

            // Merge Sort
            File result(name_ + ".bk", O_CREAT | O_TRUNC | O_RDWR, DEFAULT_FILE_MODE);
            result.setFileSize(elem_count_ * sizeof(Pair));
            FVector vec(elem_count_,  Allocator<Pair>(result.getFd()));

            for (std::size_t i = 0; i < elem_count_; ++i) {
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
            index_count_ = elem_count_ / elem_per_page_ + (elem_count_ % elem_per_page_ ? 1 : 0);
            
            FVector vec(elem_count_, Allocator<Pair>(data_.getFd()));

            File index(index_path, O_CREAT | O_TRUNC | O_RDWR, DEFAULT_FILE_MODE);
            index.setFileSize(index_count_ * sizeof(Index));

            IVector vec_i(index_count_,  Allocator<Index>(index.getFd()));
            for (long long i = 0; i < index_count_; ++i) {
                vec_i[i].key    = vec[i * elem_per_page_].key;
                vec_i[i].offset = i * elem_per_page_ * sizeof(Pair);
            }

            index_ = std::move(vec_i);
        }

        bool isSorted() {
            return std::is_sorted(vec_data_.begin(), vec_data_.end(), [] (const Pair& x, const Pair& y) -> bool {
                return x.key < y.key;        
            });
        }

        void setIndex(const std::string& index_path) { 
            File index(index_path, O_RDWR, DEFAULT_FILE_MODE);
            IVector vec_i(index_count_,  Allocator<Index>(index.getFd()));
        
            index_count_ = index.getSize() / sizeof(Index);
            
            index_      = std::move(vec_i);
            index_file_ = std::move(index);
        }

        std::optional<Data> search(uint64_t hash_key) {
            if (index_count_ == -1) {
                throw std::runtime_error("Index file doesn't exist");
            }
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

            std::size_t elem_offset = (--it)->offset / sizeof(Pair);
            std::size_t rest =  elem_count_ - elem_offset;
            std::size_t elem = rest < elem_per_page_ ? rest : elem_per_page_;

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
        std::size_t elem_count_;
        std::size_t elem_per_page_;
        long long index_count_ = -1;

        File index_file_;
        IVector index_;

        std::string name_;

        File data_;
        FVector vec_data_;
};

#endif  // SYSTEM_FILE_H_
