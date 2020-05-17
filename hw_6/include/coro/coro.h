#ifndef CORO_CORO_H_
#define CORO_CORO_H_

#include <ucontext.h>
#include <functional>
#include <memory>

#include "non_copyable.h"

#include <iostream>

static constexpr std::size_t ROUTINE_STACK_SIZE = 1 << 20;

namespace Network::Coro {

using routine_t = std::size_t;
using RoutineFunc = std::function<void()>;

void yield();
void entry();

struct Ordinator;

class Routine : public NonCopyable {
    public:
        Routine();
        Routine(RoutineFunc&& routine);
        Routine(Routine&& routine);
        Routine& operator=(Routine&& routine);
        ~Routine();

        bool resume();

        void reset(RoutineFunc&& routine);
        void again();

        bool isFinished() const;
        bool isWorking() const;

    private:
        bool is_finished_;
        bool is_working_;

        ucontext_t routine_ctx_;
        std::unique_ptr<uint8_t[]> stack_;
        
        RoutineFunc routine_;
        std::exception_ptr exception_;

        friend struct Ordinator;
        friend void entry();
        friend void yield();
};


}  // Network::Coro

#endif  // CORO_CORO_H_
