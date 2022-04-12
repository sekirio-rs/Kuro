#include <coroutine>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include "liburing.h"

template <typename T>
class Op {
 public:
  using Callback = std::function<void(struct io_uring_sqe*, __s32* res)>;

  __u64 token;
  __s32 res;
  std::shared_ptr<io_uring> uring_handle;
  Callback cb;

  // Awaitable needed
  bool await_ready();
  void await_suspend(std::coroutine_handle<> h);
  auto await_resume();
  
  Op(T val, std::shared_ptr<io_uring>& uring, __u64 token, Callback f);

 private:
  T value;
};
