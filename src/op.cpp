#include "kuro.h"

thread_local std::map<unsigned long, std::coroutine_handle<>> CO_HANDLES;
thread_local std::map<unsigned long, __s32*> URING_RESULTS;

template <typename T>
bool Op<T>::await_ready() {
  return false;
}

template <typename T>
void Op<T>::await_suspend(std::coroutine_handle<> h) {
  struct io_uring* handle = uring_handle.get();
  struct io_uring_sqe* sqe = io_uring_get_sqe(handle);

  cb(sqe);

  io_uring_sqe_set_data(sqe, (void*)token);

  CO_HANDLES.insert(std::pair{token, h});
  URING_RESULTS.insert(std::pair{token, &res});
}

template <typename T>
auto Op<T>::await_resume() {
  CO_HANDLES.erase(token);
  return res;
}

template <typename T>
Op<T>::Op(const T val, std::shared_ptr<io_uring>& uring, Callback f)
    : value(val), cb(f) {
  uring_handle = uring;
  token = (unsigned long)this;
}

// todo: remove it
template class Op<int>;
