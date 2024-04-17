#!/bin/sh

echo "@cmake" &&
cmake \
    -S ./ \
    -B ./build/ \
    -G "Ninja" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc \
    &&
echo "" &&
echo "@cmake build" &&
cmake --build ./build &&
echo "" &&
echo "@run" &&
./build/VIS
