#include <arpa/inet.h>
#include <string.h>

#include "kuro.h"

TcpListener::TcpListener() {
  int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);

  if (fd < 0) throw std::runtime_error("socket(2) return value less than zero");

  sockfd = fd;
}

TcpListener::TcpListener(int sockfd) : sockfd(sockfd) {}

TcpListener::~TcpListener() { close(sockfd); }

void TcpListener::set_reuseaddr(bool reuseaddr) {
  int opt = reuseaddr ? 1 : 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt,
                 sizeof(opt)) < 0)
    throw std::runtime_error("set_reuseaddr error");
}

void TcpListener::set_reuseport(bool reuseport) {
  int opt = reuseport ? 1 : 0;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, (const void*)&opt,
                 sizeof(opt)) < 0)
    throw std::runtime_error("set_reuseport error");
}

void TcpListener::bind_socket(const char* ip_addr,
                              unsigned short int sin_port) {
  struct in_addr sin_addr;

  if (inet_aton(ip_addr, &sin_addr) == 0)
    throw std::runtime_error("ip address error");

  addr.sin_family = AF_INET;
  addr.sin_addr = sin_addr;
  addr.sin_port = sin_port;
  bzero(&(addr.sin_zero), sizeof(addr.sin_zero));

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1)
    throw std::runtime_error("bind(2) return -1");

  return;
}

void TcpListener::listen_socket(int backlog) {
  if (listen(sockfd, backlog) == -1)
    throw std::runtime_error("listen(2) return -1");
}

Map<__s32, Accept, int> TcpListener::async_accept(
    std::shared_ptr<io_uring>& uring, TcpStream* stream_) {
  auto accept = Accept(uring, sockfd, &stream_->addr, &stream_->len, 0);
  int* stream_fd_ptr = &stream_->fd;

  auto map = Map<__s32, Accept, int>(std::move(accept),
                                     [stream_fd_ptr](__s32 fd) -> int {
                                       *stream_fd_ptr = (int)fd;
                                       return (int)fd;
                                     });

  return map;
}
