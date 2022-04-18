#include "kuro.h"

Read::Read(std::shared_ptr<io_uring>& uring, const int fd, void* buf,
           unsigned nbytes, __u64 offset)
    : fd(fd), buf(buf), nbytes(nbytes), offset(offset) {
  uring_handle = uring;
  token = (unsigned long)this;

  cb = [fd, buf, nbytes, offset](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_read(sqe, fd, buf, nbytes, offset);
  };
}
