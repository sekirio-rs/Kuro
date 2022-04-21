#include "kuro.h"

Accept::Accept(std::shared_ptr<io_uring>& uring, int sockfd,
               struct sockaddr* addr, socklen_t* len, int flags)
    : sockfd(sockfd), addr(addr), len(len), flags(flags) {
  uring_handle = uring;

  cb = [sockfd, addr, len, flags](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_accept(sqe, sockfd, addr, len, flags);
  };
}
