#include "kuro.h"
#include <unistd.h>

File::File(int fd) : fd(fd) {}

File::~File() { close(fd); }
