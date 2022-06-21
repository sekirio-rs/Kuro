<img src="./assets/kuro.jpg" width=300 high=200 align="left">

# Kuro
Linux io_uring based c++ 20 coroutine library

## What is "Kuro"?
`Kuro` is a character in Game [Sekiro](https://www.sekirothegame.com/) by [FromSoftware](https://www.fromsoftware.jp/ww/).

In this case, it refers to a basic coroutine library, which
relies on C++ 20 coroutine syntax and Linux io_uring feature,
aiming to build easy, flexible and high-performance
`asynchronous programming framework` in C++.

## Example
```C++
Task<int> co_echo(std::shared_ptr<io_uring>& handle) {
  void* buf;
  TcpListener listener = TcpListener();
  TcpStream stream_ = TcpStream();

  listener.bind_socket("127.0.0.1", htons(3344));
  
  listener.listen_socket(1024);

  co_await listener.async_accept(handle, &stream_);
  
  if(posix_memalign(&buf, 1024, 1024))
    co_return 1;

  while(1) {
    int n = co_await stream_.async_recv(handle, buf, 1024);

    if (n == 0) break;

    co_await stream_.async_send(handle, buf, n);
  }

  co_return 0;
}
```

## Build
```shell
git clone https://github.com/sekirio-rs/Kuro
cd Kuro
git submodule update --init

make # build kuro.so
make example # build examples
```

## How to use
`Kuro` works as a dynamic library(.so).

First, generate the `kuro.so`:
```shell
cd Kuro
make
# then kuro.so will be in Kuro/build
```

Then `include "kuro.h"` in your code(xxx.cc) and compile it:
```shell
g++ -fcoroutines -std=c++20 -Wall -O2 -D_GNU_SOURCE xxx.cc -o xxx -luring -lkuro
```

**WARNING**:
* Make sure `liburing.so.*` be prepared in your system to successfully compiled. See https://github.com/axboe/liburing to setup it.
* Checkout your Linux kernel version. >= 5.13 will be 100% OK, otherwise may `-22` be returned on some IO operations.
* Checkout your gcc(g++) version, make sure it support C++ 20 and coroutine.


> See how `Makefile` build examples for compiling details.

## License
Copyright (C) 2022 SKTT1Ryze. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under License.

