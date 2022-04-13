#include "op.h"

#define QD 4

int main() {
  std::cout << "hello, kuro" << std::endl;

  int val = 1;
  __s32* res_ptr;
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }

  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  auto op = Op(val, handle, 0, [&res_ptr](struct io_uring_sqe*, __s32* res) -> void{ res_ptr = res; });
  return 0;
}
