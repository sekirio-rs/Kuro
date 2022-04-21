#include "kuro.h"

Send::Send(std::shared_ptr<io_uring>& uring, int sockfd, const void* buf,
           size_t len, int flags)
    : sockfd(sockfd), buf(buf), len(len), flags(flags) {
  uring_handle = uring;

  cb = [sockfd, buf, len, flags](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_send(sqe, sockfd, buf, len, flags);
  };
}
