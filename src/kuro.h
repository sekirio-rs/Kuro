#include <sys/uio.h>

#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

#include "future.h"
#include "liburing.h"

using Callback = std::function<void(struct io_uring_sqe*)>;

/* ----- Coroutine ----- */

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
  ~Task() {
    // h_.destroy();
  }

  T result() { return h_.promise().value; }
};

void async_execute(std::shared_ptr<io_uring>& uring_handle);

/* ----- Events ----- */

template <typename T>
class Op : public Future<__s32> {
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

class Readv : public Op<int> {
 public:
  int fd;
  const struct iovec* iov;
  unsigned nr_vecs;
  __u64 offset;

  Readv(std::shared_ptr<io_uring>& uring, const int fd, const struct iovec* iov,
        unsigned nr_vecs, __u64 offset);
};

class Write : public Op<int> {
 public:
  int fd;
  const void* buf;
  unsigned nbytes;
  __u64 offset;

  Write(std::shared_ptr<io_uring>& uring, const int fd, const void* buf,
        unsigned nbytes, __u64 offset);
};

class Writev : public Op<int> {
 public:
  int fd;
  const struct iovec* iov;
  unsigned nr_vecs;
  __u64 offset;

  Writev(std::shared_ptr<io_uring>& uring, const int fd,
         const struct iovec* iov, unsigned nr_vecs, __u64 offset);
};

class OpenAt : public Op<int> {
 public:
  int dfd;
  const char* path;
  int flags;
  mode_t mode;

  OpenAt(std::shared_ptr<io_uring>& uring, int dfd, const char* path, int flags,
         mode_t mode);
};

/* ----- File System ----- */
class File {
 public:
  File() {}
  File(int fd);
  ~File();

  Read read(std::shared_ptr<io_uring>& uring, void* buf, unsigned nbytes);
  Readv readv(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
              unsigned nr_vecs);
  Write write(std::shared_ptr<io_uring>& uring, const void* buf,
              unsigned nbytes);
  Writev writev(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
                unsigned nr_vecs);

 private:
  int fd;
};

Map<__s32, OpenAt, int> async_open(std::shared_ptr<io_uring>& uring,
                                   const char* path);

// failed to be compiled because of bug of gcc
//
// Map<__s32, OpenAt, File> async_open(std::shared_ptr<io_uring>& uring,
//                                   const char*path);

Map<__s32, OpenAt, int> async_create(std::shared_ptr<io_uring>& uring,
                                     const char* path);
