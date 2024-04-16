#!/bin/sh

echo "-> CMAKE SETUP" &&
cmake \
    -S ./ \
    -B ./build/ \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER=g++ \
    -DCMAKE_C_COMPILER=gcc \
    &&
echo "" &&
echo "-> CMAKE BUILD" &&
cmake --build ./build &&
echo "" &&
echo "-> RUN" &&
./build/VIS
