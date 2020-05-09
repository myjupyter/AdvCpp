#ifndef CORO_CORO_H_
#define CORO_CORO_H_

#include <ucontext.h>
#include <functional>
#include <memory>

#include "non_copyable.h"

static constexpr std::size_t ROUTINE_STACK_SIZE = 1 << 20;

namespace Network::Coro {

using routine_t = std::size_t;
using RoutineFunc = std::function<void()>;

void yield();
void entry();

class Routine : public NonCopyable {
    public:
        Routine();
        Routine(RoutineFunc&& routine);
        Routine(Routine&& routine);
        Routine& operator=(Routine&& routine);
        ~Routine() = default;

        bool resume();

        void reset(RoutineFunc&& routine);

    public:
        bool is_working_;

        ucontext_t routine_ctx_;
        std::unique_ptr<uint8_t[]> stack_;
        
        RoutineFunc routine_;
        std::exception_ptr exception_;
};


}  // Network::Coro

#endif  // CORO_CORO_H_
