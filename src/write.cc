#include "kuro.h"

Write::Write(std::shared_ptr<io_uring>& uring, const int fd, const void* buf,
             unsigned nbytes, __u64 offset)
    : fd(fd), buf(buf), nbytes(nbytes), offset(offset) {
  uring_handle = uring;

  cb = [fd, buf, nbytes, offset](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_write(sqe, fd, buf, nbytes, offset);
  };
}

Writev::Writev(std::shared_ptr<io_uring>& uring, const int fd,
               const struct iovec* iov, unsigned nr_vecs, __u64 offset)
    : fd(fd), iov(iov), nr_vecs(nr_vecs), offset(offset) {
  uring_handle = uring;

  cb = [fd, iov, nr_vecs, offset](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_writev(sqe, fd, iov, nr_vecs, offset);
  };
}
