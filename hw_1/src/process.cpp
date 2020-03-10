#include "process.h"

#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <unistd.h>

namespace process {

Process::Process(const std::string& path) {
    
    int pipe1[2] = {-1, -1};
    int pipe2[2] = {-1, -1};
    if (pipe(pipe1) < 0 || 
        pipe(pipe2) < 0) {
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

        stdin = fdopen(STDIN_FILENO, "r");
        stdout = fdopen(STDOUT_FILENO, "w");
        if (stdin == NULL || stdout == NULL) {
            throw std::runtime_error(std::strerror(errno));
        }

        char* args[2] = {(char*) path.c_str(), NULL};        
        if (execv(args[0], args) == -1) {
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
    this->close(); 
    if (isWorking()) {
        kill(proc_pid_, SIGKILL); 
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
    if (len != this->write(data, len)) {
        throw std::runtime_error("Not all bytes were sent");
    }
}
 
std::size_t Process::read(void* data, std::size_t len) {
    if (!isWorking() || !isReadable()) {
        return 0;
    }

    int bytes = ::read(proc_stdout_, data, len);
    if (bytes == -1) {
        throw std::runtime_error(std::strerror(errno));
    } else if (bytes == 0) {
         proc_stdout_stat_ = DescStat::IS_CLOSED;
    }
    return static_cast<std::size_t>(bytes);
}

void Process::readExact(void* data, std::size_t len) {
    if (len != this->read(data, len)) {
        throw std::runtime_error("Not all bytes were recieved");
    }
}

bool Process::isReadable() const {
    return static_cast<bool>(proc_stdout_stat_);
}

bool Process::isWritable() const {
    return static_cast<bool>(proc_stdin_stat_);
}

void Process::closeStdin() {
    ::close(proc_stdin_);
    proc_stdin_stat_ = DescStat::IS_CLOSED;
}

void Process::close() {
    ::close(proc_stdin_);
    ::close(proc_stdout_);

    proc_stdin_stat_ = DescStat::IS_CLOSED;
    proc_stdout_stat_ = DescStat::IS_CLOSED;
}

bool Process::isWorking() const {
    int status = -1;
    pid_t result = waitpid(proc_pid_, &status, WNOHANG);
    if (result == 0) {
        return true;
    }
    return false;
}

};  // namespace process
