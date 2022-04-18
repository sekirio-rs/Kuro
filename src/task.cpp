#include "kuro.h"

extern thread_local std::map<unsigned long, std::coroutine_handle<>> CO_HANDLES;
extern thread_local std::map<unsigned long, __s32*> URING_RESULTS;

void async_execute(std::shared_ptr<io_uring>& uring_handle, int task_count) {
  struct io_uring* uring = uring_handle.get();
  int count = 0;

  while (true) {
    struct io_uring_cqe* cqe;
    unsigned head;
    int ret = io_uring_submit_and_wait(uring, 1);

    if (count >= task_count) break;

    if (ret < 0) {
      throw std::runtime_error("io_uring_submit_and_wait error");  // todo
      break;
    }

    io_uring_for_each_cqe(uring, head, cqe) {
      __s32 res = cqe->res;

      // if (res < 0) {
      //   throw std::runtime_error("cqe res less then zero");  // todo
      //   break;
      // }

      unsigned long token = static_cast<unsigned long>(cqe->user_data);

      *(URING_RESULTS.find(token)->second) = res;  // set result

      CO_HANDLES.find(token)->second.resume();
      count++;
    }
  }
  return;
}
