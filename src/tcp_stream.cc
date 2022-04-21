#include "kuro.h"

TcpStream::TcpStream(int fd) : fd(fd) {}

TcpStream::~TcpStream() { close(fd); }

Recv TcpStream::async_recv(std::shared_ptr<io_uring>& uring, void* buf,
                           size_t len) {
  return Recv(uring, fd, buf, len, 0);
}

Send TcpStream::async_send(std::shared_ptr<io_uring>& uring, const void* buf,
                           size_t len) {
  return Send(uring, fd, buf, len, 0);
}
