#include "kuro.h"
#include <fcntl.h>

#define QD 4

// for test
Task<int> co_read(std::shared_ptr<io_uring>& handle) {
  int fd = open("README.md", O_RDONLY | O_DIRECT);
  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
    co_return -1;
  }

  char buf[1024] = { 0 };

  auto read = Read(handle, fd, buf, 1024, 0);
  __s32 res = co_await read;
  
  std::cout << buf << std::endl;

  co_return 0;
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
  
  async_execute(handle, 1);

  return 0;
}

