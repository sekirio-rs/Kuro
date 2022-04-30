# Kuro
Linux io_uring based c++ 20 coroutine library

## What is "Kuro"?
`Kuro` is a character in Game `Sekiro` by `From Software`.

In this case, it refers to a basic coroutine library, which
relies on C++ 20 coroutine syntax and Linux io_uring feature,
aiming to build easy, flexible and high performance
`asynchorous programming framework` in C++.

## Build
```shell
make # build kuro.so
make example # build examples
```

## How to use
`Kuro` works as a dynamic link library(.so).

First, generate the `kuro.so`:
```shell
cd Kuro
make
// then kuro.so will be in Kuro/build
```

Then `include "kuro.h"` in your code(xxx.cc) and compile it:
```shell
g++ -fcoroutines -std=c++20 -Wall -O2 -D_GNU_SOURCE xxx.cc -o xxx -luring -lkuro
```

**WARNING**: make sure liburing.so.* be prepared in your system.

> See how `Makefile` build examples for usage details.

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

