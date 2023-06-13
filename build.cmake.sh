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

u="$USER"
if [[ "$u" != "root" ]] ;
then
 cmake .. -DWT_INCLUDE=/home/pvn/wt_install/include -DWT_CONFIG_H=/home/pvn/wt_install/include -DBUILD_WEB=ON
else
 cmake .. -DWT_ROOT=ON -DWT_INCLUDE=/root/wt_install/include -DWT_CONFIG_H=/root/wt_install/include -DBUILD_WEB=ON
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
cmake .. -DWT_INCLUDE=/Users/pvn/wt_install/include -DWT_CONFIG_H=/Users/pvn/wt_install/include -DBUILD_WEB=ON

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DBUILD_STATIC=OFF -DWT_INCLUDE=N:/nostr_client_relay/ext/wt-4.10.0/src -DWT_CONFIG_H=N:/nostr_client_relay/ext/wt-4.10.0/build -DBUILD_WEB=ON
cmake --build .

fi
cmake --build . --parallel 9
popd
pwd

pushd build
pushd web
export LD_LIBRARY_PATH=/home/pvn/git/nostr_client_relay/ext/boost_1_82_0/stage/lib:$LD_LIBRARY_PATH
./nostro_web --http-address=0.0.0.0 --http-port=8080  --docroot=.
exit

