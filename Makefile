# Makefile for Kuro

B = build
S = src
E = examples

CC = g++
CFLAGS = -fcoroutines -std=c++20
CFLAGS += -Wall -O2 -D_GNU_SOURCE
CFLAGS += -I./liburing/src/include/
CFLAGS += -I./src/
SO_CFLAGS = -fPIC -c
LDFLAGS = -L./$B/ -luring -lkuro

FORMAT = clang-format
FFLAGS = --style=google -i
LIB_URING = $B/liburing.so
LIB_OBJ = $B/libkuro.o
LIB = $B/libkuro.so

$(LIB): $(LIB_OBJ)
	$(CC) -shared $(LIB_OBJ) -o $(LIB)

$(LIB_OBJ):
	@if [ ! -d $B ]; then mkdir $B; fi
	$(CC) $(CFLAGS) $(SO_CFLAGS) src/op.cpp -o $(LIB_OBJ)

$(LIB_URING):
	@if [ ! -d $B ]; then mkdir $B; fi
	@cd ./liburing && make && cp src/liburing.so.* ../$B/liburing.so || exit 1

uring: $(LIB_URING)

EXAMPLE_SRC = $(shell echo $E/*.cpp)
EXAMPLE_DST = $(patsubst $E/%, %, $(patsubst %.cpp, %, $(EXAMPLE_SRC)))

$(EXAMPLE_DST): $(LIB_URING) $(LIB)
	@if [ ! -d $B ]; then mkdir $B; fi
	$(CC) $(CFLAGS) $(patsubst %, $E/%.cpp, $@) -o $B/$@ $(LDFLAGS)

example: $(EXAMPLE_DST)

format-example: $(EXAMPLE_SRC)
	@$(FORMAT) $(FFLAGS) $<

format:
	@$(FORMAT) $(FFLAGS) $S/*
clean:
	@rm -rf $B
