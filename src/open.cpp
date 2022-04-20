#include "kuro.h"

OpenAt::OpenAt(std::shared_ptr<io_uring>& uring, int dfd, const char* path,
               int flags, mode_t mode)
    : dfd(dfd), path(path), flags(flags), mode(mode) {
  uring_handle = uring;

  cb = [dfd, path, flags, mode](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_openat(sqe, dfd, path, flags, mode);
  };
}
