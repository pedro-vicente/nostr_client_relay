#!/bin/bash

opt=ON
if [[ "$OSTYPE" == "msys" ]]; then
dir=$PWD
fi

mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" \
-DBUILD_WEB=$opt -DBUILD_DESKTOP=$opt -DBUILD_MOBILE=$opt

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" \
-DBUILD_WEB=$opt -DBUILD_DESKTOP=$opt -DBUILD_MOBILE=$opt

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DBUILD_STATIC=OFF -DWT_INCLUDE="$dir/ext/wt-4.10.0/src" -DWT_CONFIG_H="$dir/ext/wt-4.10.0/build" \
-DBUILD_WEB=$opt -DBUILD_DESKTOP=$opt -DBUILD_MOBILE=$opt
cmake --build .

fi

sleep 3
cmake --build .

popd
pwd

pushd build
pushd web

remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
sleep 2
echo "open browser http://localhost:8080"
if [ "$remote" == "https://github.com/pedro-vicente/nostr_client_relay.git" ]; then
export LD_LIBRARY_PATH="$HOME/github/nostr_client_relay/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
else
export LD_LIBRARY_PATH="$HOME/git/nostr_client_relay/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
fi
if [[ "$OSTYPE" == "msys"* ]]; then
./Debug/wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
else
./wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
fi

exit

