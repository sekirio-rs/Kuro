#include <coroutine>

template <typename T>
class Future {
 public:
   // Awaitable in C++, Future in Rust
   bool await_ready();
   void await_suspend(std::coroutine_handle<> h);
   T await_resume();
};
