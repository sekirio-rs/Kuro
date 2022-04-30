/*
 * Copyright (C) 2022 SKTT1Ryze. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under License.
 * */

#include "kuro.h"

Read::Read(std::shared_ptr<io_uring>& uring, const int fd, void* buf,
           unsigned nbytes, __u64 offset)
    : fd(fd), buf(buf), nbytes(nbytes), offset(offset) {
  uring_handle = uring;

  cb = [fd, buf, nbytes, offset](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_read(sqe, fd, buf, nbytes, offset);
  };
}

Readv::Readv(std::shared_ptr<io_uring>& uring, const int fd,
             const struct iovec* iov, unsigned nr_vecs, __u64 offset)
    : fd(fd), iov(iov), nr_vecs(nr_vecs), offset(offset) {
  uring_handle = uring;

  cb = [fd, iov, nr_vecs, offset](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_readv(sqe, fd, iov, nr_vecs, offset);
  };
}
