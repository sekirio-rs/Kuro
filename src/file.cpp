#include <fcntl.h>
#include <unistd.h>

#include "kuro.h"

File::File(int fd) : fd(fd) {}

File::~File() { close(fd); }

Map<__s32, OpenAt, int> kuro_open(std::shared_ptr<io_uring>& uring,
                                  const char* path) {
  OpenAt open = OpenAt(uring, AT_FDCWD, path, O_RDONLY, 0600);
  auto map =
      Map<__s32, OpenAt, int>(open, [](__s32 fd) -> int { return (int)fd; });
  return map;
}
