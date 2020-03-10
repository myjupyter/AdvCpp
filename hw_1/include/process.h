#ifndef PROCESS_H
#define PROCESS_H

#include <string>

namespace process {

enum DescStat {IS_CLOSED = 0, IS_OPENED};

class Process {
    public:
        explicit Process(const std::string& path);
        ~Process();

        std::size_t write(const void* data, std::size_t len);
        void writeExact(const void* data, std::size_t len);
        std::size_t read(void* data, std::size_t len);
        void readExact(void* data, std::size_t len);

        bool isReadable() const;
        bool isWritable() const;
        void closeStdin();

        void close();
        
        // checks whether the child process is working
        bool isWorking() const;
    private:

        pid_t proc_pid_;        

        int proc_stdin_;
        int proc_stdout_;
        
        DescStat proc_stdout_stat_;
        DescStat proc_stdin_stat_;
};

};  // namespace process

#endif  //PROCESS_H

