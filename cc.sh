#!/bin/sh
CFLAGS="-std=c23 -Wall -Wextra -pedantic -fsanitize=address -fsanitize=undefined -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wstrict-overflow=5 -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code -Wpointer-arith -Wuninitialized -Winit-self -march=native -O3 -g -Wdouble-promotion -Wformat -Wformat-security -Wnull-dereference -Wimplicit-fallthrough -Wunused -Wstrict-overflow=5"
clang $CFLAGS $@
