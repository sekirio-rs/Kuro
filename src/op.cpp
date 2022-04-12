#include "op.h"

template <typename T>
bool Op<T>::await_ready() {
  return false;
}

template <typename T>
void Op<T>::await_suspend(std::coroutine_handle<> h) {
  struct io_uring* handle = uring_handle.get();
  struct io_uring_sqe* sqe = io_uring_get_sqe(handle);

  cb(sqe, &res);

  io_uring_sqe_set_data(sqe, static_cast<void*>(&token));
}

template <typename T>
auto Op<T>::await_resume() {
  return res;
}

template <typename T>
Op<T>::Op(T val, std::shared_ptr<io_uring>& uring, __u64 token, Callback f)
    : value(val), token(token), cb(f) {
  uring_handle = uring;
}
