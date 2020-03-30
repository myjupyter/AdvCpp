#ifndef NON_COPYABLE_H_
#define NON_COPYABLE_H_

namespace Network {

class NonCopyable {
    protected:
        NonCopyable() = default;
        ~NonCopyable() = default;
        NonCopyable(NonCopyable&&) = default;
        NonCopyable& operator=(NonCopyable&&) = default;

    private: 
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
};

}

#endif  // NON_COPYABLE_H_
