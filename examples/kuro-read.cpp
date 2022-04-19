#include "kuro.h"
#include <fcntl.h>

#define QD 4
#define BUF_LEN 1024

Task<int> co_readv(std::shared_ptr<io_uring>& handle) {
  struct iovec iov;
  void* buf;
  
  auto open = async_open(handle, "README.md");
  int fd = co_await open;
  
  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
    co_return -1;
  }

  if(posix_memalign(&buf, BUF_LEN, BUF_LEN))
    co_return 1;

  iov.iov_base = buf;
  iov.iov_len = BUF_LEN;

  auto readv = Readv(handle, fd, &iov, 1, 0);
  __s32 _res = co_await readv;
  
  std::cout << "readv: " << std::endl << (char*)buf << std::endl;

  co_return 0;
}

Task<int> co_read(std::shared_ptr<io_uring>& handle) {
  void* buf;
  
  auto open = async_open(handle, "README.md");
  int fd = co_await open;
  
  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
    co_return -1;
  }

  if(posix_memalign(&buf, BUF_LEN, BUF_LEN))
    co_return 1;
  
  auto read = Read(handle, fd, buf, BUF_LEN, 0);
  __s32 _res = co_await read;
  
  // auto file = File(fd);
  // auto read = file.read(handle, buf, BUF_LEN);
  // __s32 _res = co_await read;

  std::cout << "read: " << std::endl << (char*)buf << std::endl;

  co_return 0;
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  auto task0 = co_readv(handle);
  auto task1 = co_read(handle);

  async_execute(handle);
  
  return 0;
}

