#ifndef CORO_CORO_H_
#define CORO_CORO_H_

#include <ucontext.h>

#include <list>
#include <vector>
#include <functional>
#include <memory>

#include "non_copyable.h"

static constexpr std::size_t ROUTINE_STACK_SIZE = 1 << 20;

namespace Network::Coro {

using routine_t = std::size_t;
using Routine = std::function<void()>:

class Coroutine;

thread_local struct Ordinator {
    ucontext_t main_ctx = {};
    routine_t current_routine = 0;
    
    std::vector<Coroutine> coroutines;
    std::list<routine_t> finished_rotine;
} ordinator;

class Coroutine : public NonCopyable {
    public:
        Coroutine() = delete;
        Сoroutine(Routine routine);
        ~Coroutine();

        void reset(const Routine& routine);

        bool resume();

    private:
        bool is_finished_;
        Routine routine_;

        ucontext_t routine_ctx_;
        std::unique_ptr<std::byte*> stack_;
};

}  // Network::Coro

#endif  // CORO_CORO_H_
