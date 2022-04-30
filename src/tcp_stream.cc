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

TcpStream::TcpStream(int fd) : fd(fd) {}

TcpStream::~TcpStream() { close(fd); }

Recv TcpStream::async_recv(std::shared_ptr<io_uring>& uring, void* buf,
                           size_t len) {
  return Recv(uring, fd, buf, len, 0);
}

Send TcpStream::async_send(std::shared_ptr<io_uring>& uring, const void* buf,
                           size_t len) {
  return Send(uring, fd, buf, len, 0);
}
