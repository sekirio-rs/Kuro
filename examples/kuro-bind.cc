#include <iostream>
#include "kuro.h"

#define QD 4

Task<TcpListener> co_bind(std::shared_ptr<io_uring>& handle) {
  TcpListener listener = TcpListener();
  
  listener.bind_socket("127.0.0.1", 3344);

  std::cout << "binded" << std::endl;

  co_return listener;
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);
  
  auto task = co_bind(handle);

  async_execute(handle);

  auto listener = task.result();

  return 0;
}
