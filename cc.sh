#!/bin/sh
CFLAGS="-std=c2x -Wall -Wextra -pedantic -fsanitize=address -fsanitize=undefined -Wfloat-equal -Wundef -Wshadow -Wcast-align -Wstrict-prototypes -Wwrite-strings -Wstrict-overflow=5 -Wcast-qual -Wswitch-enum -Wconversion -Wunreachable-code -Wpointer-arithmetic -Wuninitialized -Winit-self -fmudflap -march=native -O3 -g"
cc $CFLAGS $@
