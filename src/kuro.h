#include <coroutine>
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
  auto await_resume();

  Op() {}
  Op(const T val, std::shared_ptr<io_uring>& uring, Callback f);

 private:
  T value;
};

class Read : Op<int> {
 public:
  int fd;
  void* buf;
  unsigned nbytes;
  __u64 offset;

  Read(std::shared_ptr<io_uring>& uring, const int fd, void* buf,
       unsigned nbytes, __u64 offset);
};
