#include "process.h"

#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <unistd.h>

#include <iostream>

namespace process {
 
Process::Process(const std::string& path,
                 const std::vector<std::string>& args) {

    int pipe1[2];
    int pipe2[2];
    if (pipe(pipe1) < 0) { 
        throw std::runtime_error(std::strerror(errno));
    }
    if (pipe(pipe2) < 0) {
        ::close(pipe1[0]);
        ::close(pipe1[1]);
        throw std::runtime_error(std::strerror(errno));
    }
    
    int parent_write = pipe1[1];
    int child_read = pipe1[0];

    int child_write = pipe2[1];
    int parent_read = pipe2[0];

    proc_pid_ = fork();
    if (proc_pid_ < 0) {
        throw std::runtime_error(std::strerror(errno));
    } else if (proc_pid_ == 0) {
        ::close(parent_write);
        ::close(parent_read);

        if (dup2(child_read, STDIN_FILENO) == -1 ||
            dup2(child_write, STDOUT_FILENO) == -1) {
            throw std::runtime_error(std::strerror(errno));
        }
        ::close(child_read);
        ::close(child_write);

        std::vector<char*> argv;
        argv.push_back(const_cast<char*>(path.c_str()));
        for (const std::string& arg: args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        argv.push_back(nullptr);

        if (execv(argv[0], argv.data()) == -1) {
            throw std::runtime_error(std::strerror(errno));
        }
    } else if (proc_pid_ > 0) {
        ::close(child_read);
        ::close(child_write);

        proc_stdin_ = parent_write;
        proc_stdout_ = parent_read;

        proc_stdin_stat_ = DescStat::IS_OPENED;
        proc_stdout_stat_ = DescStat::IS_OPENED;
    }
}

Process::~Process() {  
    close();
    if (isWorking()) {
        int status = -1;   
        ::kill(proc_pid_, SIGTERM);
        wait(status, WEXITED);
    }
}

std::size_t Process::write(const void* data, std::size_t len) {
    if (!isWorking() || !isWritable()) {
        return 0;
    }

    ssize_t bytes = ::write(proc_stdin_, data, len);
    if (bytes == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
    return static_cast<std::size_t>(bytes);
}

void Process::writeExact(const void* data, std::size_t len) {
    std::size_t rest = 0;
    std::size_t old_rest = 0;

    while (rest != len) {
        old_rest = rest;
        rest += write(static_cast<const char*>(data) + rest, len - rest);
        if ((rest - old_rest) == 0) {
            throw std::runtime_error(std::to_string(rest) + \
                    "/" + std::to_string(len) + " bytes were sent");
        }
    }
}
 
std::size_t Process::read(void* data, std::size_t len) {
    if (!isWorking() || !isReadable()) {
        return 0;
    }

    ssize_t bytes = ::read(proc_stdout_, data, len);
    if (bytes == -1) {
        throw std::runtime_error(std::strerror(errno));
    } else if (bytes == 0) {
         proc_stdout_stat_ = DescStat::IS_CLOSED;
    }
    return static_cast<std::size_t>(bytes);
}

void Process::readExact(void* data, std::size_t len) {
    std::size_t rest = 0;
    std::size_t old_rest = 0;

    while (rest != len) {
        old_rest = rest;
        rest += read(static_cast<char*>(data) + rest, len - rest);
        if ((rest - old_rest) == 0) {
            throw std::runtime_error(std::to_string(rest) + \
                    "/" + std::to_string(len) + " bytes were received");
        }
    }
}

bool Process::isReadable() const {
    return proc_stdout_stat_ == DescStat::IS_OPENED;
}

bool Process::isWritable() const {
    return proc_stdin_stat_ == DescStat::IS_OPENED;
}

void Process::closeStdin() {
    ::close(proc_stdin_);
    proc_stdin_stat_ = DescStat::IS_CLOSED;
}

pid_t Process::wait(int& status, int options) const {
    return waitpid(proc_pid_, &status, options);
}

void Process::close() {
    ::close(proc_stdin_);
    ::close(proc_stdout_);

    proc_stdin_stat_ = DescStat::IS_CLOSED;
    proc_stdout_stat_ = DescStat::IS_CLOSED;
}

bool Process::isWorking() const {
    int status = -1;
    pid_t result = wait(status, WNOHANG);
    if (result != 0 && WIFEXITED(status)) {
        return false;
    }
    return true;
}

}  // namespace process
