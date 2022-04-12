#include <coroutine>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <map>
#include "liburing.h"

template <typename T>
class Op {
 public:
  unsigned token;
  std::shared_ptr<io_uring> uring_handle;

  // Awaitable needed
  bool await_ready();
  void await_suspend(std::coroutine_handle<> h);
  auto await_resume();

  Op(T val, std::shared_ptr<io_uring> uring);

 private:
  T value;
};
