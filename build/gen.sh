#!/bin/sh

export CFLAGS="-g -Wall -Wextra -pedantic"

mkdir -p linux && \
cd linux && \
cmake \
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
-DUSE_SPLINT:BOOL=OFF \
-DENABLE_REPEAT:BOOL=OFF \
-G "Unix Makefiles" ../
