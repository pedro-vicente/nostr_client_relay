#!/bin/bash
mkdir -p build
pushd build
cmake .. 
sleep 2
cmake --build . 
popd
exit


