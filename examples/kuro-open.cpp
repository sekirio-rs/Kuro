#include "kuro.h"

#define QD 4
#define BUF_LEN 10

Task<File> co_open(std::shared_ptr<io_uring>& handle) {
  auto open = kuro_open(handle, "README.md");    
  auto fd = co_await open;
  co_return File(fd);
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

  auto task = co_open(handle);

  async_execute(handle);
  
  File file = task.result();
  return 0;
}

