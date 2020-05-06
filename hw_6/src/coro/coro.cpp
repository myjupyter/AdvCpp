#include "coro.h"
#include <cerrno>

#include <cstring>

namespace Network::Coro {

thread_local struct Ordinator {
    ucontext_t thread_ctx = {};
    routine_t current = 0;
    routine_t routine_id = 0;

    Routine routine;
} ordinator;

routine_t create(routine_t id, const RoutineFunc& func) {
    ordinator.routine.reset(func);
    ordinator.routine_id = id;
    return id; 
}

bool resume(routine_t id) {
    if (ordinator.routine_id != id) {
        return false; 
    }

    if (ordinator.routine.is_finished_ || 
        ordinator.routine.is_working_) {
        return false;
    }

    ordinator.routine.is_working_ = true;
    ordinator.current = id;

    if (::swapcontext(&ordinator.thread_ctx, &ordinator.routine.routine_ctx_) == -1) {
        ordinator.current = 0;
        throw std::runtime_error(std::strerror(errno));
    }
    
    ordinator.current = 0;
    ordinator.routine.is_working_ = false;

    if (ordinator.routine.exception_) {
        std::rethrow_exception(ordinator.routine.exception_);
    }
    return true;
}

void yield() {
   ordinator.current = 0;
    if (::swapcontext(&ordinator.routine.routine_ctx_, &ordinator.thread_ctx) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
}

void entry() {
    if (ordinator.routine.routine_) try {
        ordinator.routine.routine_();
    } catch (...) {
        ordinator.routine.exception_ = std::current_exception();
    }

    ordinator.routine.is_finished_ = true;
    ordinator.current = 0;

    if (::swapcontext(&ordinator.routine.routine_ctx_, &ordinator.thread_ctx) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
}

Routine::Routine()  
    : is_finished_(false)
    , is_working_(false)
    , stack_(std::make_unique<uint8_t[]>(ROUTINE_STACK_SIZE))
    , exception_{} {
    routine_ctx_.uc_stack.ss_sp = stack_.get();
    routine_ctx_.uc_stack.ss_size = ROUTINE_STACK_SIZE;
    routine_ctx_.uc_link = &ordinator.thread_ctx;

    ::getcontext(&routine_ctx_);
    ::makecontext(&routine_ctx_, entry, 0);    
}


Routine::Routine(const RoutineFunc& routine) 
    : Routine() {
    routine_ = routine;    
}

Routine::Routine(Routine&& routine) {
    is_finished_ = routine.is_finished_;
    is_working_ = routine.is_working_;

    stack_ = std::move(routine.stack_);
    routine_ = std::move(routine.routine_);

    ::memmove(&routine_ctx_, &routine.routine_ctx_, sizeof(ucontext_t));
    routine.routine_ctx_ = {};
}

Routine& Routine::operator=(Routine&& routine) {
    if (this == &routine) {
        return *this;
    }
    is_finished_ = routine.is_finished_;
    is_working_ = routine.is_working_;

    stack_ = std::move(routine.stack_);
    routine_ = std::move(routine.routine_);

    ::memmove(&routine_ctx_, &routine.routine_ctx_, sizeof(ucontext_t));
    routine.routine_ctx_ = {};
    return *this;
}


void Routine::reset(const RoutineFunc& routine) {
    is_finished_ = false;
    is_working_ = false;

    routine_ = routine;
    exception_ = {};
    
    ::makecontext(&routine_ctx_, entry, 0);
}

}  // Network::Coro
