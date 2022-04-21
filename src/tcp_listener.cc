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

void TcpListener::bind_socket(const char* ip_addr,
                              unsigned short int sin_port) {
  struct sockaddr_in addr;
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
