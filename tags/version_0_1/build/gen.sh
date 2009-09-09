#!/bin/sh

mkdir -p linux && \
cd linux && \
cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON -G "Unix Makefiles" ../
