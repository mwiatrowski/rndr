#!/usr/bin/env bash

clang-format -i src/* &&
mkdir -p build &&
cd build &&
cmake .. &&
make &&
./rndr
