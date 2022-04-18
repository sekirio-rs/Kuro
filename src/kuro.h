#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include "liburing.h"

using Callback = std::function<void(struct io_uring_sqe*)>;

template <typename T>
class Op {
 public:
  __s32 res;
  std::shared_ptr<io_uring> uring_handle;
  Callback cb;
  unsigned long token;

  // Awaitable needed
  bool await_ready();
  void await_suspend(std::coroutine_handle<> h);
  __s32 await_resume();

  Op() {}
  Op(const T val, std::shared_ptr<io_uring>& uring, Callback f);

 private:
  T value;
};

class Read : public Op<int> {
 public:
  int fd;
  void* buf;
  unsigned nbytes;
  __u64 offset;

  Read(std::shared_ptr<io_uring>& uring, const int fd, void* buf,
       unsigned nbytes, __u64 offset);
};

template <typename T>
class Task {
 public:
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  struct promise_type {
    std::exception_ptr exception_;
    T value;

    Task get_return_object() { return Task(handle_type::from_promise(*this)); }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    // void return_void() {}
    std::suspend_never return_value(T v) {
      value = v;

      if (exception_) std::rethrow_exception(exception_);

      return {};
    }
    void unhandled_exception() { exception_ = std::current_exception(); }
  };

  handle_type h_;

  Task(handle_type h) : h_(h) {}
  ~Task() { h_.destroy(); }

  T result() { return h_.promise().value; }
};

void async_execute(std::shared_ptr<io_uring>& uring_handle, int task_num);