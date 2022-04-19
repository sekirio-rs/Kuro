#include "kuro.h"
#include <fcntl.h>
#include <memory>

#define QD 4
#define BUF_LEN 10

Task<int> co_write(std::shared_ptr<io_uring>& handle) {
  const char buf[] = "Hello,Kuro";
  
  int fd = open("temp.md", O_RDWR | O_CREAT, 0600);
  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
    co_return -1;
  }
  
  auto write = Write(handle, fd, buf, BUF_LEN, 0);
  __s32 _res = co_await write;

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

