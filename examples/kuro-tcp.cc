#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>

#include "kuro.h"

#define QD 4
#define BACKLOG 10
#define BUF_LEN 1024

Task<int> co_echo(std::shared_ptr<io_uring>& handle) {
  void* buf;
  TcpListener listener = TcpListener();
  TcpStream stream_ = TcpStream();

  listener.bind_socket("127.0.0.1", htons(3344));
  
  listener.listen_socket(BACKLOG);

  co_await listener.async_accept(handle, &stream_);
  
  if(posix_memalign(&buf, BUF_LEN, BUF_LEN))
    co_return 1;

  while(1) {
    int n = co_await stream_.async_recv(handle, buf, BUF_LEN);

    if (n == 0) break;

    co_await stream_.async_send(handle, buf, BUF_LEN);
  }

  co_return 0;
}

int main() {
  struct io_uring ring;

  if (io_uring_queue_init(QD, &ring, 0) < 0) {
    std::cout << "io_uring_queue_init error" << std::endl;
    return 1;
  }
  
  std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);
  
  auto task = co_echo(handle);

  async_execute(handle);

  return 0;
}
