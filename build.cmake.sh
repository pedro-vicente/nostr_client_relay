#!/bin/bash
mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
cmake ..
cmake --build . --target configurator
pushd ext
pushd secp256k1-0.3.2
make
popd
popd
elif [[ "$OSTYPE" == "darwin"* ]]; then
cmake ..
cmake --build . --target configurator
pushd ext
pushd secp256k1-0.3.2
make
popd
popd
elif [[ "$OSTYPE" == "msys" ]]; then
cmake .. --fresh
cmake --build . 
fi
cmake --build .
popd
exit

