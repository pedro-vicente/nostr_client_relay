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

pwd

if [ $1 -a $1 != "root" ] ;
then
 cmake .. -DWT_INCLUDE=/home/pvn/wt_install/include -DWT_CONFIG_H=/home/pvn/wt_install/include
else
 cmake .. -DWT_ROOT=ON -DWT_INCLUDE=/root/wt_install/include -DWT_CONFIG_H=/root/wt_install/include
fi

elif [[ "$OSTYPE" == "darwin"* ]]; then

pwd
cmake .. 
cmake --build . --target configurator
pushd ext
pushd secp256k1-0.3.2
make
popd
popd

pwd
cmake .. -DWT_INCLUDE=/Users/pvn/wt_install/include -DWT_CONFIG_H=/Users/pvn/wt_install/include

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DBUILD_STATIC=OFF -DWT_INCLUDE=N:/nostr_client_relay/ext/wt-4.10.0/src -DWT_CONFIG_H=N:/nostr_client_relay/ext/wt-4.10.0/build 
cmake --build .

fi
cmake --build . --parallel 9
popd
pwd
exit

