/*
 * Copyright (C) 2022 SKTT1Ryze. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under License.
 * */

#if !defined(__cpp_impl_coroutine)
#define __cpp_impl_coroutine 1
#endif

#include <linux/in.h>
#include <sys/socket.h>
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

/* ----- C++20 Coroutine ----- */

/*
 * Coroutine is usually treated as resumable function in C++
 * and asynchorous task in Rust.
 *
 * [`Task`] here refers to return type of coroutine function:
 * ```C++
 * Task<> co_xxx(...) {
 *  co_await xxx;
 *
 *  co_return xxx;
 * }
 * ```
 *
 * More details about C++20 coroutine in
 * https://en.cppreference.com/w/cpp/language/coroutines
 *
 * More details about Kuro Task in `Kuro/examples`
 * */
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

/*
 * Enter event loop and execute created [`Task<T>`]s
 * until all of then being handled.
 *
 * example:
 * ```C++
 * Task<> co_read() {...}
 *
 * // prepare io_uring
 * struct io_uring uring;
 * io_uring_queue_init(4, &ring, 0);
 * std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);
 *
 * // create two tasks
 * auto task0 = co_read();
 * auto task1 = co_read();
 *
 * // asynchorous execute
 * async_execute(handle);
 *
 * // both of task0 and task1 have been done.
 *
 * ```
 *
 * More details in `Kuro/examples`
 * */
void async_execute(std::shared_ptr<io_uring>& uring_handle);

/* ----- IO Events ----- */

/*
 * Base class of all io events
 *
 * [`Op<T>`] is subclass of [`Future<T>`], which is abstract inteface of
 * Awaitable in C++ 20 coroutine.
 *
 * `Awaitable` in C++ is similar to `Future` in Rust.
 *
 * More details about Awaitable/Future in
 * https://en.cppreference.com/w/cpp/language/coroutines and
 * https://rust-lang.github.io/async-book/02_execution/02_future.html
 * */
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

/*
 * Asynchorous and awaitable object equal to read(2).
 *
 * example:
 * ```C++
 * Task<int> co_read(void* buf, shared_ptr<io_uring>& uring) {
 *  int fd = open(...);
 *
 *  int n = co_await Read(uring, fd, buf, 1024, 0);
 *
 *  printf("read %d bytes\n", n);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Read : public Op<int> {
 public:
  int fd;
  void* buf;
  unsigned nbytes;
  __u64 offset;

  Read(std::shared_ptr<io_uring>& uring, const int fd, void* buf,
       unsigned nbytes, __u64 offset);
};

/*
 * Asynchorous and awaitable object equal to readv(2).
 *
 * example:
 * ```C++
 * Task<int> co_readv(const struct iovec* iov, shared_ptr<io_uring>& uring) {
 *  int fd = open(...);
 *
 *  int n = co_await Readv(uring, fd, iov, 1, 0);
 *
 *  printf("readv %d bytes\n", n);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Readv : public Op<int> {
 public:
  int fd;
  const struct iovec* iov;
  unsigned nr_vecs;
  __u64 offset;

  Readv(std::shared_ptr<io_uring>& uring, const int fd, const struct iovec* iov,
        unsigned nr_vecs, __u64 offset);
};

/*
 * Asynchorous and awaitable object equal to write(2).
 *
 * example:
 * ```C++
 * Task<int> co_write(const void* buf, shared_ptr<io_uring>& uring) {
 *  int fd = open(...);
 *
 *  int n = co_await Write(uring, fd, buf, 1024, 0);
 *
 *  printf("write %d bytes\n", n);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Write : public Op<int> {
 public:
  int fd;
  const void* buf;
  unsigned nbytes;
  __u64 offset;

  Write(std::shared_ptr<io_uring>& uring, const int fd, const void* buf,
        unsigned nbytes, __u64 offset);
};

/*
 * Asynchorous and awaitable object equal to writev(2).
 *
 * example:
 * ```C++
 * Task<int> co_writev(const struct iovec* iov, shared_ptr<io_uring>& uring) {
 *  int fd = open(...);
 *
 *  int n = co_await Write(uring, fd, iov, 1, 0);
 *
 *  printf("writev %d bytes\n", n);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Writev : public Op<int> {
 public:
  int fd;
  const struct iovec* iov;
  unsigned nr_vecs;
  __u64 offset;

  Writev(std::shared_ptr<io_uring>& uring, const int fd,
         const struct iovec* iov, unsigned nr_vecs, __u64 offset);
};

/*
 * Asynchorous and awaitable object equal to openat(2).
 *
 * example:
 * ```C++
 * Task<int> co_openat(const char* path, shared_ptr<io_uring>& uring) {
 *  int fd = co_await OpenAt(uring, AT_FDCWD, path, O_RDONLY, 0600);
 *
 *  printf("open %d\n", fd);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class OpenAt : public Op<int> {
 public:
  int dfd;
  const char* path;
  int flags;
  mode_t mode;

  OpenAt(std::shared_ptr<io_uring>& uring, int dfd, const char* path, int flags,
         mode_t mode);
};

/*
 * Asynchorous and awaitable object equal to accept(2).
 *
 * example:
 * ```C++
 * Task<int> co_accept(int sockfd, shared_ptr<io_uring>& uring) {
 *  struct sockaddr addr;
 *  socklen_t len;
 *
 *  co_await Accept(uring, sockfd, &addr, &len, 0);
 *
 *  printf("accepted");
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Accept : public Op<int> {
 public:
  int sockfd;
  struct sockaddr* addr;
  socklen_t* len;
  int flags;

  Accept(std::shared_ptr<io_uring>& uring, int sockfd, struct sockaddr* addr,
         socklen_t* len, int flags);
};

/*
 * Asynchorous and awaitable object equal to recv(2).
 *
 * example:
 * ```C++
 * Task<int> co_recv(int sockfd, void* buf, shared_ptr<io_uring>& uring) {
 *  int n = co_await Recv(uring, sockfd, buf, 1024, 0);
 *
 *  printf("recv %d bytes\n");
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Recv : public Op<int> {
 public:
  int sockfd;
  void* buf;
  size_t len;
  int flags;

  Recv(std::shared_ptr<io_uring>& uring, int sockfd, void* buf, size_t len,
       int flags);
};

/*
 * Asynchorous and awaitable object equal to send(2).
 *
 * example:
 * ```C++
 * Task<int> co_send(int sockfd, const void* buf, shared_ptr<io_uring>& uring) {
 *  int n = co_await Send(uring, sockfd, buf, 1024, 0);
 *
 *  printf("send %d bytes\n");
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in https://github.com/axboe/liburing/tree/master/man.
 * */
class Send : public Op<int> {
 public:
  int sockfd;
  const void* buf;
  size_t len;
  int flags;

  Send(std::shared_ptr<io_uring>& uring, int sockfd, const void* buf,
       size_t len, int flags);
};

/* ----- File System ----- */

/*
 * The class holds a file descriptor.
 *
 * It's clever to perform all file operations via [`File`] and
 * it's methond.
 *
 * example:
 * ```C++
 * Task<int> co_fs(std::shared_ptr<io_uring>& uring) {
 *  int fd = co_await async_open(uring, "xxx");
 *  auto file = File(fd);
 *
 *  char buf[50] = "Hello, Kuro";
 *
 *  co_await file.write(uring, buf, strlen(buf));
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in `Kuro/examples`.
 * */
class File {
 public:
  File() {}
  File(int fd);
  ~File();

  /*
   * Asynchorous and awaitable read at file equal to read(2).
   *
   * See examples in [`Read`] and
   * https://man7.org/linux/man-pages/man2/read.2.html
   * */
  Read read(std::shared_ptr<io_uring>& uring, void* buf, unsigned nbytes);

  /*
   * Asynchorous and awaitable readv at file equal to readv(2).
   *
   * See examples in [`Readv`] and
   * https://man7.org/linux/man-pages/man2/readv.2.html
   * */
  Readv readv(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
              unsigned nr_vecs);

  /*
   * Asynchorous and awaitable write at file equal to write(2).
   *
   * See examples in [`Write`] and
   * https://man7.org/linux/man-pages/man2/write.2.html
   * */
  Write write(std::shared_ptr<io_uring>& uring, const void* buf,
              unsigned nbytes);

  /*
   * Asynchorous and awaitable writev at file equal to writev(2).
   *
   * See examples in [`Writev`] and
   * https://man7.org/linux/man-pages/man2/writev.2.html
   * */
  Writev writev(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
                unsigned nr_vecs);

 private:
  int fd;
};

/*
 * Asynchorous open file with read-only mode.
 *
 * `WARNING`: the type of return value is [`int`] instead of [`File`].
 *
 * Originally, we can co_await [`File`] type like:
 * ```C++
 * File f = co_await async_open(...);
 * ```
 *
 * if we change the [`Map<__s32, OpenAt, int>`] to [`Map<__s32, OpenAt, File>`]
 * like:
 * ```C++
 * Map<__s32, OpenAt, File> async_open(...);
 * ```
 *
 * However, bug in `gcc` prevents us to do that:
 *  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=101133.
 *
 * Hope it be fixed soon:).
 * */
Map<__s32, OpenAt, int> async_open(std::shared_ptr<io_uring>& uring,
                                   const char* path);

// failed to be compiled because of bug of gcc
//
// Map<__s32, OpenAt, File> async_open(std::shared_ptr<io_uring>& uring,
//                                   const char*path);

/*
 * Asynchorous create file with read-write mode.
 * */
Map<__s32, OpenAt, int> async_create(std::shared_ptr<io_uring>& uring,
                                     const char* path);

/* ----- Net ----- */

/*
 * [`TcpStream`] holds a socket to perform send/recv operations.
 *
 * example:
 * ```C++
 * Task<int> co_send(std::shared_ptr<io_uring>& uring, TcpStream* stream_) {
 *  void* buf;
 *
 *  if(posix_memalign(&buf, 1024, 1024))
 *  co_return 1;
 *
 *  int n = co_await stream_.async_recv(uring, buf, 1024);
 *
 *  printf("recv %d bytes\n", n);
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in `Kuro/examples`.
 * */
class TcpStream {
 public:
  int fd;
  struct sockaddr addr;
  socklen_t len;

  TcpStream(){};
  TcpStream(int fd);
  ~TcpStream();

  /*
   * Asynchorous and awaitable recv at stream equal to recv(2).
   *
   * See examples in [`Recv`] and
   * https://man7.org/linux/man-pages/man2/recv.2.html
   * */
  Recv async_recv(std::shared_ptr<io_uring>& uring, void* buf, size_t len);

  /*
   * Asynchorous and awaitable send at stream equal to send(2).
   *
   * See examples in [`Send`] and
   * https://man7.org/linux/man-pages/man2/send.2.html
   * */
  Send async_send(std::shared_ptr<io_uring>& uring, const void* buf,
                  size_t len);
};

/*
 * [`TcpListener`] holds a socket to accept connections.
 *
 * example:
 * ```C++
 * Task<int> co_accept(std::shared_ptr<io_uring>& uring) {
 *  TcpListener listener = TcpListener();
 *  TcpStream stream_ = TcpStream();
 *
 *  listener.bind_socket("127.0.0.1", htons(3344));
 *  listener.listen_socket(1024);
 *
 *  co_await listener.async_accept(uring, &stream_);
 *
 *  printf("accepted");
 *
 *  co_return 0;
 * }
 * ```
 *
 * More details in `Kuro/examples`.
 * */
class TcpListener {
 public:
  TcpListener();
  TcpListener(int sockfd);
  ~TcpListener();

  /*
   * Configure ip address as reusable.
   * */
  void set_reuseaddr(bool reuseaddr);

  /*
   * Configure ip port as reusable.
   * */
  void set_reuseport(bool reuseport);

  /*
   * Bind the socket to specific ip address and port.
   * */
  void bind_socket(const char* ip_addr, unsigned short int sin_port);

  /*
   * Listen on the specific ip address and port.
   * */
  void listen_socket(int backlog);

  /*
   * Asynchorous and awaitable accept a connection equal to accept(2).
   *
   * See examples in [`Accept`] and
   * https://man7.org/linux/man-pages/man2/accept.2.html
   * */
  Map<__s32, Accept, int> async_accept(std::shared_ptr<io_uring>& uring,
                                       TcpStream* stream_);

 private:
  int sockfd;
  struct sockaddr_in addr;
};
