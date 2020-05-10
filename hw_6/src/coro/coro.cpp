#include "coro.h"
#include <cerrno>

#include <cstring>
#include <iostream>
#include <thread>

namespace Network::Coro {

thread_local struct Ordinator {
    ucontext_t thread_ctx = {};
    Routine* routine = nullptr;
} ordinator;

void yield() {
    if (ordinator.routine == nullptr) {
        return;
    }
    if (::swapcontext(&ordinator.routine->routine_ctx_, &ordinator.thread_ctx) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }
}

void entry() {
    if (ordinator.routine == nullptr) {
        throw std::runtime_error("null");
    }

    if (ordinator.routine->routine_ ) try {
        ordinator.routine->routine_();
        ordinator.routine->is_finished_ = true;
    } catch (...) {
        ordinator.routine->exception_ = std::current_exception();
    }
    if (::swapcontext(&ordinator.routine->routine_ctx_, &ordinator.thread_ctx) == -1) {
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

    routine_ = nullptr;    
}

Routine::Routine(RoutineFunc&& routine) 
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

bool Routine::resume() {
    
    ordinator.routine = this;

    if (ordinator.routine->is_finished_) {
        again();
    }

    if (ordinator.routine->is_working_) {
        return false;
    }

    ordinator.routine->is_working_ = true;

    if (::swapcontext(&ordinator.thread_ctx, &ordinator.routine->routine_ctx_) == -1) {
        ordinator.routine = nullptr;
        throw std::runtime_error(std::strerror(errno));
    }

    ordinator.routine->is_working_ = false;
    
    if (ordinator.routine->exception_) {
        std::rethrow_exception(ordinator.routine->exception_);
    }
    
    ordinator.routine = nullptr;

    return true;
}

void Routine::reset(RoutineFunc&& routine) {
    is_finished_ = false;
    is_working_ = false;

    routine_ = routine;
    exception_ = {};
    
    ::makecontext(&routine_ctx_, entry, 0);
}

void Routine::again() {
    is_finished_ = false;
    is_working_ = false;

    exception_ = {};

    ::makecontext(&routine_ctx_, entry, 0);
}

}  // Network::Coro
