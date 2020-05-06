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

routine_t create(routine_t id, const RoutineFunc& func);
bool resume(routine_t id);
void yield();
routine_t current();
void entry();


template <typename T, typename F, typename ...Args, typename = std::enable_if_t<!std::is_invocable_v<F>>> 
routine_t create(T&& t, F&& f, Args&&... args) {
    return create(std::forward<T>(t), std::bind(std::forward<F>(f), std::forward<Args>(args)...));
}

template <typename T, typename F, typename ...Args> 
bool create_and_run(T&& t, F&& f, Args&&... args) {
    return resume(create(std::forward<T>(t), std::forward<F>(f), std::forward<Args>(args)...));
}

class Routine : public NonCopyable {
    public:
        Routine();
        Routine(const RoutineFunc& routine);
        Routine(Routine&& routine);
        Routine& operator=(Routine&& routine);
        ~Routine() = default;

        void reset(const RoutineFunc& routine);

//  private:
        bool is_finished_;
        bool is_working_;

        ucontext_t routine_ctx_;
        std::unique_ptr<uint8_t[]> stack_;
        
        RoutineFunc routine_;
        std::exception_ptr exception_;
};


}  // Network::Coro

#endif  // CORO_CORO_H_
