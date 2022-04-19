#include "kuro.h"
#include <fcntl.h>

#define QD 4
#define BUF_LEN 1

// for test
Task<int> co_read(std::shared_ptr<io_uring>& handle) {
  int fd = open("README.md", O_RDONLY | O_DIRECT);
  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
    co_return -1;
  }

  char buf[BUF_LEN] = { 0 };
  struct iovec iov = {iov_base: (void*)buf, iov_len: BUF_LEN};

  auto readv = Readv(handle, fd, &iov, 1, 0);
  __s32 res = co_await readv;
  
  std::cout << "readv res: " << res << std::endl;

  co_return 0;
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  auto task = co_read(handle);
  
  async_execute(handle);
  
  return 0;
}

