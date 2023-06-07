#!/bin/bash

git clone --recursive --depth=1 https://github.com/boostorg/boost.git ext/boost_1_82_0
git checkout boost-1.82.0
sleep 5

pushd ext
pushd boost_1_82_0

if [[ "$OSTYPE" == "msys" ]]; then
./bootstrap.bat
./b2 --prefix=.  --layout=versioned --toolset=msvc-14.3 address-model=64 architecture=x86 variant=debug threading=multi link=static runtime-link=shared install

elif [[ "$OSTYPE" == "darwin"* ]]; then

./bootstrap.sh --prefix=`pwd`
./b2 --prefix=`pwd` variant=release headers

elif [[ "$OSTYPE" == "linux-gnu"* ]]; then

./bootstrap.sh
./b2 --prefix=. variant=release 

fi
popd
popd

#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/Users/pvn/git/nostro_web/boost_1_82_0
#The following directory should be added to linker library paths:
#/Users/pvn/git/nostro_web/boost_1_82_0/stage/lib


#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/home/pvn/git/nostro_web/boost_1_82_0
#The following directory should be added to linker library paths:
#/home/pvn/git/nostro_web/boost_1_82_0/stage/lib
