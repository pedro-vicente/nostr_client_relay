#!/bin/bash

if [[ "$OSTYPE" == "msys" ]]; then
dir=$PWD
fi

mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" \
-DBUILD_WEB=OFF -DBUILD_DESKTOP=OFF -DBUILD_MOBILE=OFF

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" \
-DBUILD_WEB=OFF -DBUILD_DESKTOP=OFF -DBUILD_MOBILE=OFF

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DBUILD_STATIC=OFF -DWT_INCLUDE="$dir/ext/wt-4.10.0/src" -DWT_CONFIG_H="$dir/ext/wt-4.10.0/build" \
-DBUILD_WEB=OFF -DBUILD_DESKTOP=OFF -DBUILD_MOBILE=OFF
cmake --build .

fi

sleep 3
cmake --build .

popd
exit

