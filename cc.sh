#!/bin/sh
CFLAGS="-std=c23 -Wall -Wextra -pedantic -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wstrict-overflow=5 -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code -Wpointer-arith -Wuninitialized -Winit-self -march=native -O3 -g -Wdouble-promotion -Wformat -Wformat-security -Wnull-dereference -Wimplicit-fallthrough -Wunused -Wstrict-overflow=5"
CLANG_CFLAGS="-ferror-limit=2 -fsanitize=address -fsanitize=undefined"
GCC_CFLAGS="-fmax-errors=2"
CC="gcc"
gcc $GCC_CFLAGS $CFLAGS $@ && ./a.out
