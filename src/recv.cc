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

Recv::Recv(std::shared_ptr<io_uring>& uring, int sockfd, void* buf, size_t len,
           int flags)
    : sockfd(sockfd), buf(buf), len(len), flags(flags) {
  uring_handle = uring;

  cb = [sockfd, buf, len, flags](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_recv(sqe, sockfd, buf, len, flags);
  };
}
