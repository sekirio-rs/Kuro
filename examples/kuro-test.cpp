#include "kuro.h"

#define QD 4

// for test
task co_read(std::shared_ptr<io_uring>& handle) {
  int fd = 0;
  void* buf = nullptr;

  auto read = Read(handle, fd, buf, 0, 0);
  __s32 res = co_await read;

  co_return;
}

int main() {
  std::cout << "hello, kuro" << std::endl;

  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  co_read(handle);

  return 0;
}

