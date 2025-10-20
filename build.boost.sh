#!/bin/bash
set -e
remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
 
if [ ! -d "ext/boost_1_88_0" ]; then
    git -c advice.detachedHead=false clone -b boost-1.88.0 --recursive --depth=1 https://github.com/boostorg/boost.git ext/boost_1_88_0
    rm -rf build/wt-4.12.0
else
    echo "ext/boost_1_88_0 already exists, skipping clone"
fi

sleep 2
mkdir -p build/boost_1_88_0
pushd ext
pushd boost_1_88_0

if [[ "$OSTYPE" == "msys" ]]; then
./bootstrap.bat
./b2 --prefix=../../build/boost_1_88_0  --layout=versioned --toolset=msvc-14.3 address-model=64 architecture=x86 variant=debug threading=multi \
  link=static runtime-link=shared \
--build-dir=../../build/boost_1_88_0 --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono install

elif [[ "$OSTYPE" == "darwin"* ]]; then

./bootstrap.sh
./b2 --prefix=../../build/boost_1_88_0 --layout=versioned variant=release threading=multi link=static runtime-link=shared \
--build-dir=../../build/boost_1_88_0 --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono install

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

./bootstrap.sh
./b2 --prefix=../../build/boost_1_88_0 --layout=versioned variant=release threading=multi link=static runtime-link=shared \
--build-dir=../../build/boost_1_88_0 cxxflags=-fPIC --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono install

fi

popd
popd

