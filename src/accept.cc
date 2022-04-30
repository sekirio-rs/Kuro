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

Accept::Accept(std::shared_ptr<io_uring>& uring, int sockfd,
               struct sockaddr* addr, socklen_t* len, int flags)
    : sockfd(sockfd), addr(addr), len(len), flags(flags) {
  uring_handle = uring;

  cb = [sockfd, addr, len, flags](struct io_uring_sqe* sqe) -> void {
    io_uring_prep_accept(sqe, sockfd, addr, len, flags);
  };
}
