#include "kuro.h"

TcpStream::TcpStream(int fd) : fd(fd) {}

TcpStream::~TcpStream() { close(fd); }
