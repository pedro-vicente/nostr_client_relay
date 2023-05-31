#!/bin/bash

# build dependencies first
mkdir -p build/ext/wxWidgets-3.2.2.1
pushd build
pushd ext
pushd wxWidgets-3.2.2.1
#WX (-DwxBUILD_SAMPLES=SOME | ALL | OFF )
cmake ../../../ext/wxWidgets-3.2.2.1 -DwxBUILD_SHARED=OFF -DwxBUILD_SAMPLES=OFF -DBUILD_STATIC=OFF  \
-DwxUSE_LIBTIFF=OFF
sleep 3
cmake --build . --parallel 9
popd
popd
popd
