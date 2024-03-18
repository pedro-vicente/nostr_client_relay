#!/bin/bash
mkdir -p build
pushd build
cmake ..
cmake --build .

if [[ "$OSTYPE" == "msys" ]]; then
  pushd Debug
  pwd
  ./examples.exe
  popd
else
 ./examples
fi

popd
pwd
exit

