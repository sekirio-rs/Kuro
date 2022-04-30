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

OpenAt::OpenAt(std::shared_ptr<io_uring>& uring, int dfd, const char* path,
               int flags, mode_t mode)
    : dfd(dfd), path(path), flags(flags), mode(mode) {
  uring_handle = uring;

  cb = [dfd, path, flags, mode](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_openat(sqe, dfd, path, flags, mode);
  };
}
