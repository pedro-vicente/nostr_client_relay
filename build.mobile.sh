#!/bin/bash
mkdir -p build
pushd build
if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DBUILD_MOBILE=ON
cmake --build . 

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DBUILD_MOBILE=ON \
-DCMAKE_PREFIX_PATH=$QT_IOS_DIR \
-DDEPLOYMENT_TARGET=14.0 \
-DPLATFORM=OS64COMBINED \
-DENABLE_BITCODE=FALSE \
-DCMAKE_BUILD_TYPE=Release \
-DAPPLE_IOS_TARGET=True \
-DQT_HOST_PATH=/Users/pvn/Qt6.5.2/6.5.2/macos \
-DQT_DEBUG_FIND_PACKAGE=ON \
-G Xcode \
-DCMAKE_TOOLCHAIN_FILE=mobile/ios.toolchain.cmake \
-DPLATFORM=OS64

cmake --build . --config Release

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. -DBUILD_MOBILE=ON -DCMAKE_PREFIX_PATH="F:\\qt6\\qt-everywhere-src-6.5.2\\qtbase\\bin"
cmake --build . 

fi

popd
pwd
