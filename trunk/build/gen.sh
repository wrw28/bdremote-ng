#!/bin/sh

export CFLAGS="-Wall -Wextra -pedantic"

mkdir -p linux && \
cd linux && \
cmake \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
-DUSE_SPLINT:BOOL=OFF \
-G "Unix Makefiles" ../
