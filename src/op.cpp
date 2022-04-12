#include "op.h"

template <typename T>
bool Op<T>::await_ready() {
  return false;
}

template <typename T>
void Op<T>::await_suspend(std::coroutine_handle<> h) {
  // todo
}
