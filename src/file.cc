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

#include <fcntl.h>
#include <unistd.h>

#include "kuro.h"

File::File(int fd) : fd(fd) {}

File::~File() { close(fd); }

Read File::read(std::shared_ptr<io_uring>& uring, void* buf, unsigned nbytes) {
  return Read(uring, fd, buf, nbytes, 0);
}

Readv File::readv(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
                  unsigned nr_vecs) {
  return Readv(uring, fd, iov, nr_vecs, 0);
}

Write File::write(std::shared_ptr<io_uring>& uring, const void* buf,
                  unsigned nbytes) {
  return Write(uring, fd, buf, nbytes, 0);
}

Writev File::writev(std::shared_ptr<io_uring>& uring, const struct iovec* iov,
                    unsigned int nr_vecs) {
  return Writev(uring, fd, iov, nr_vecs, 0);
}

Map<__s32, OpenAt, int> async_open(std::shared_ptr<io_uring>& uring,
                                   const char* path) {
  OpenAt open = OpenAt(uring, AT_FDCWD, path, O_RDONLY, 0600);
  auto map = Map<__s32, OpenAt, int>(std::move(open),
                                     [](__s32 fd) -> int { return (int)fd; });
  return map;
}

Map<__s32, OpenAt, int> async_create(std::shared_ptr<io_uring>& uring,
                                     const char* path) {
  OpenAt create = OpenAt(uring, AT_FDCWD, path, O_RDWR | O_CREAT, 0600);
  auto map = Map<__s32, OpenAt, int>(std::move(create),
                                     [](__s32 fd) -> int { return (int)fd; });

  return map;
}
