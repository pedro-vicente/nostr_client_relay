#!/bin/bash
mkdir -p build
pushd build
cmake ..
cmake --build .
popd
pwd
exit

