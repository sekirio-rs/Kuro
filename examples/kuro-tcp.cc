#include <iostream>
#include <arpa/inet.h>
#include "kuro.h"

#define QD 4
#define BACKLOG 1024

Task<int> co_bind(std::shared_ptr<io_uring>& handle) {
  TcpListener listener = TcpListener();
  TcpStream stream_ = TcpStream();

  listener.bind_socket("127.0.0.1", htons(3344));
  
  listener.listen_socket(BACKLOG);

  int fd = co_await listener.async_accept(handle, &stream_);

  std::cout << "listen with fd: " << fd << std::endl;

  co_return 0;
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

  return 0;
}
