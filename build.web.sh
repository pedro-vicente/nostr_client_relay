#!/bin/bash
set -e
path_wt="$(pwd)/install/wt"
path_boost="$(pwd)/build/boost_1_88_0"
echo "Wt at: $path_wt"
echo "Boost at: $path_boost"
sleep 1

# build directory 
mkdir -p build
pushd build

cmake .. -DBUILD_WEB=ON \
    -DWT_INCLUDE="$path_wt/include" \
    -DBOOST_INCLUDE_DIR="$path_boost/include/boost-1_88" \
    -DBOOST_LIB_DIRS="$path_boost/lib"
cmake --build .  --verbose

pushd web
echo "open browser http://localhost:8080"
if [[ "$OSTYPE" == "msys"* ]]; then
./Debug/wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
else
./wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
fi

exit

