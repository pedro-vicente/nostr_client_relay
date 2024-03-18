#!/bin/bash
mkdir -p build
pushd build
cmake .. -DBUILD_DESKTOP=ON
cmake --build .
pwd
exit

