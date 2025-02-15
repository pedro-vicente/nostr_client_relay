#!/bin/bash
mkdir -p build
pushd build

if [[ "$OSTYPE" == "msys" ]]; then

cmake .. -DBUILD_DESKTOP=ON 

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DBUILD_DESKTOP=ON -DCMAKE_PREFIX_PATH="/Users/pvn/Qt/6.8.2/macos" 

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DBUILD_DESKTOP=ON \
-DCMAKE_FIND_DEBUG_MODE=ON -DQT_DEBUG_FIND_PACKAGE=ON

fi

cmake --build .

if [[ "$OSTYPE" == "linux-gnu" ]]; then

pushd desktop
pwd
./gnostro &

fi

exit

