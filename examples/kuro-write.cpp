#include "kuro.h"
#include <fcntl.h>

#define QD 4
#define BUF_LEN 10

Task<int> co_write(std::shared_ptr<io_uring>& handle) {
  struct iovec iov;
  char buf[] = "Hello,Kuro";
  
  int fd0 = co_await async_create(handle, "temp0.md");
  int fd1 = co_await async_create(handle, "temp1.md");
  if (fd0 < 0 || fd1 < 0) {
    std::cout << "open file error" << std::endl;
    co_return -1;
  }
  
  auto file0 = File(fd0);
  co_await file0.write(handle, buf, BUF_LEN);
  
  iov.iov_base = buf;
  iov.iov_len = BUF_LEN;

  auto file1 = File(fd1);
  co_await file1.writev(handle, &iov, 1);

  co_return 0;
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  auto task0 = co_write(handle);

  async_execute(handle);
  
  return 0;
}

