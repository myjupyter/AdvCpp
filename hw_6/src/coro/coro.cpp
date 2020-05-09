#include "coro.h"
#include <cerrno>

#include <cstring>

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
    auto& ord = ordinator;
    auto& rout = ord.routine;

    if (!rout) {
        return;
    }

    if (rout->routine_) try {
        rout->routine_();
    } catch (...) {
        rout->exception_ = std::current_exception();
    }

    if (::swapcontext(&rout->routine_ctx_, &ord.thread_ctx) == -1) {
        throw std::runtime_error(std::strerror(errno));
    }

    rout->reset(std::forward<RoutineFunc>(rout->routine_));
}

Routine::Routine()  
    : is_working_(false)
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
    is_working_ = routine.is_working_;

    stack_ = std::move(routine.stack_);
    routine_ = std::move(routine.routine_);

    ::memmove(&routine_ctx_, &routine.routine_ctx_, sizeof(ucontext_t));
    routine.routine_ctx_ = {};
    return *this;
}

bool Routine::resume() {
    auto& ord = ordinator;
    ord.routine = this;
    auto& rout = ord.routine;

    if (rout->is_working_) {
        return false;
    }

    rout->is_working_ = true;

    if (::swapcontext(&ord.thread_ctx, &rout->routine_ctx_) == -1) {
        ord.routine = nullptr;
        throw std::runtime_error(std::strerror(errno));
    }
    
    rout->is_working_ = false;
    
    if (rout->exception_) {
        std::rethrow_exception(rout->exception_);
    }
    
    ord.routine = nullptr;

    return true;
}

void Routine::reset(RoutineFunc&& routine) {
    is_working_ = false;

    routine_ = routine;
    exception_ = {};
    
    ::makecontext(&routine_ctx_, entry, 0);
}

}  // Network::Coro
