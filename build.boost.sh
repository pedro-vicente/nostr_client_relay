#!/bin/bash

rm -rf build/boost_1_88_0
mkdir -p build/boost_1_88_0

rm -rf ext/boost_1_88_0

git clone -b boost-1.88.0 --recursive --depth=1 https://github.com/boostorg/boost.git ext/boost_1_88_0
sleep 4

pushd ext
pushd boost_1_88_0

if [[ "$OSTYPE" == "msys" ]]; then
./bootstrap.bat
./b2 --prefix=../../build/boost_1_88_0  --layout=versioned --toolset=msvc-14.3 address-model=64 architecture=x86 variant=debug threading=multi link=static runtime-link=shared \
--build-dir=../../build/boost_1_88_0 --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono install

elif [[ "$OSTYPE" == "darwin"* ]]; then

./bootstrap.sh --prefix=`pwd`
./b2 --prefix=`pwd` variant=release --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

./bootstrap.sh
./b2 --prefix=. variant=release --with-atomic --with-date_time --with-filesystem --with-program_options --with-regex --with-thread --with-chrono

fi

popd
popd


#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/Users/pvn/git/nostro_web/boost_1_88_0
#The following directory should be added to linker library paths:
#/Users/pvn/git/nostro_web/boost_1_88_0/stage/lib


#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/home/pvn/git/nostro_web/boost_1_88_0
#The following directory should be added to linker library paths:
#/home/pvn/git/nostro_web/boost_1_88_0/stage/lib
