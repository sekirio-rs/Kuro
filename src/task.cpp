#include "kuro.h"
#include "slab.h"

extern thread_local slab<std::coroutine_handle<>> CO_HANDLES;
extern thread_local std::map<unsigned long, __s32*> URING_RESULTS;

void async_execute(std::shared_ptr<io_uring>& uring_handle) {
  struct io_uring* uring = uring_handle.get();

  while (!CO_HANDLES.is_empty() && URING_RESULTS.size()) {
    struct io_uring_cqe* cqe;
    unsigned head;
    // int pending;
    int ret = io_uring_submit(uring);

    if (ret < 0) {
      throw std::runtime_error("io_uring_submit_and_wait error");  // todo
      break;
    }

    io_uring_for_each_cqe(uring, head, cqe) {
      __s32 res = cqe->res;

      if (res < 0) {
        throw std::runtime_error("cqe res less then zero");  // todo
        break;
      }

      unsigned long token = static_cast<unsigned long>(cqe->user_data);

      *(URING_RESULTS.find(token)->second) = res;  // set result
      auto co_handle = CO_HANDLES.get(token);

      if (co_handle.has_value())
        co_handle.value()->resume();
      else
        throw std::runtime_error("co_handle empty");  // todo

      io_uring_cqe_seen(uring, cqe);
    }
  }
  return;
}
