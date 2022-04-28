#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>

#include <iostream>
#include <thread>
#include <vector>

#include "kuro.h"

#define QD 1024
#define BENCH_SIZE 1024
#define BUF_LEN 1024
#define CORES 32
#define MAX_FILE 512

Task<int> co_fio(std::shared_ptr<io_uring>& handle) {
  void* buf;

  int fd = co_await async_open(handle, "README.md");

  if (fd < 0) {
    std::cout << "open file error, fd: " << fd << std::endl;
  }

  if (posix_memalign(&buf, BUF_LEN, BUF_LEN)) co_return 1;

  auto file = File(fd);

  int n = co_await file.read(handle, buf, BUF_LEN);

  int null_fd = co_await async_create(handle, "/dev/null");

  if (null_fd < 0) {
    std::cout << "open /dev/null error." << std::endl;
  }

  auto dev_null = File(null_fd);

  co_await dev_null.write(handle, buf, n);

  co_return 0;
}

int main() {
  std::vector<std::thread> threads;

  for (int i = 0; i < CORES; i++) {
    std::thread h([] {
      struct io_uring ring;
      int bench_size = BENCH_SIZE / CORES;
      int max_file = MAX_FILE / CORES;

      if (io_uring_queue_init(QD, &ring, 0) < 0) {
        std::cout << "io_uring_queue_init error" << std::endl;
        exit(-1);
      }

      std::shared_ptr<io_uring> handle = std::make_shared<io_uring>(ring);

      for (int i = 0; i < bench_size / max_file; i++) {
        std::vector<Task<int>> tasks;
        for (int j = 0; j < max_file; j++) {
          tasks.push_back(co_fio(handle));
        }

        async_execute(handle);
      }
    });

    threads.push_back(std::move(h));
  }

  for (std::thread& h : threads) {
    if (h.joinable()) h.join();
  }

  return 0;
}
