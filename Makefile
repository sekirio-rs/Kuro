# Makefile for Kuro

B = build
S = src

CC = g++
CFLAGS = -fcoroutines -std=c++20
CFLAGS += -Wall -O2 -D_GNU_SOURCE
CFLAGS += -I./liburing/src/include/
LDFLAGS = -L./$B/ -luring

FORMAT = clang-format
FFLAGS = --style=google -i
LIB_URING = $B/liburing.so

$(LIB_URING):
	@if [ ! -d $B ]; then mkdir $B; fi
	@cd ./liburing && make && cp src/liburing.so.* ../$B/liburing.so || exit 1

uring: $(LIB_URING)

EXAMPLE_SRC = $(shell echo examples/*.cpp)
EXAMPLE_DST = $(patsubst examples%, $B%, $(patsubst %.cpp, %, $(EXAMPLE_SRC)))

$(EXAMPLE_DST): $(EXAMPLE_SRC)
	@if [ ! -d $B ]; then mkdir $B; fi
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

example: $(EXAMPLE_DST)

format-example: $(EXAMPLE_SRC)
	@$(FORMAT) $(FFLAGS) $<

format:
	@$(FORMAT) $(FFLAGS) $S/*
clean:
	rm -rf $B
