# Makefile for Kuro

B = build
S = src
E = examples

CC = g++
CFLAGS = -fcoroutines -std=c++20
CFLAGS += -Wall -O2 -D_GNU_SOURCE
CFLAGS += -I./liburing/src/include/
CFLAGS += -I./src/
SO_CFLAGS = -fPIC -shared
LDFLAGS = -L./$B/ -luring -lkuro

FORMAT = clang-format
FFLAGS = --style=google -i
LIB_URING = $B/liburing.so.2.2
LIB_URING_ = $B/liburing.so.2
LIB_URING__ = $B/liburing.so
LIB_OBJ = $B/libkuro.o
LIB = $B/libkuro.so

$(LIB):
	@if [ ! -d $B ]; then mkdir $B; fi
	$(CC) $(CFLAGS) $(SO_CFLAGS) src/op.cpp src/read.cpp src/task.cpp -o $(LIB)

$(LIB_URING):
	@if [ ! -d $B ]; then mkdir $B; fi
	@cd ./liburing && make && cp src/liburing.so.* ../$(LIB_URING) || exit 1

$(LIB_URING_): $(LIB_URING)
	@cp $(LIB_URING) $(LIB_URING_)

$(LIB_URING__): $(LIB_URING)
	@cp $(LIB_URING) $(LIB_URING__)

uring: $(LIB_URING_) $(LIB_URING__)

EXAMPLE_SRC = $(shell echo $E/*.cpp)
EXAMPLE_DST = $(patsubst $E/%, %, $(patsubst %.cpp, %, $(EXAMPLE_SRC)))

$(EXAMPLE_DST): $(LIB_URING_) $(LIB_URING__) $(LIB)
	@if [ ! -d $B ]; then mkdir $B; fi
	$(CC) $(CFLAGS) $(patsubst %, $E/%.cpp, $@) -o $B/$@ $(LDFLAGS)

example: $(EXAMPLE_DST)

format-example: $(EXAMPLE_SRC)
	@$(FORMAT) $(FFLAGS) $<

format:
	@$(FORMAT) $(FFLAGS) $S/*
clean:
	@rm -rf $B
