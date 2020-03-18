#ifndef PROCESS_H_
#define PROCESS_H_

#include <string>
#include <vector>

namespace process {

enum class DescStat {IS_CLOSED = 0, IS_OPENED};

class Process {
    public:
        Process(const Process& proc) = delete;
        Process& operator=(const Process& proc) = delete;

        explicit Process(const std::string& path);
        explicit Process(const std::vector<std::string>& args);
        ~Process();

        std::size_t write(const void* data, std::size_t len);
        void writeExact(const void* data, std::size_t len);
        std::size_t read(void* data, std::size_t len);
        void readExact(void* data, std::size_t len);

        bool isReadable() const;
        bool isWritable() const;
        void closeStdin();

        pid_t wait(int& status, int options) const;

        void close();
        
        bool isWorking() const;
    private:

        pid_t proc_pid_;        

        int proc_stdin_;
        int proc_stdout_;
        
        DescStat proc_stdout_stat_;
        DescStat proc_stdin_stat_;
};

}  // namespace process

#endif  //PROCESS_H_

