#!/bin/bash
# --http-address=0.0.0.0 --http-port=8080 --deploy-path=/hello --docroot=.
# --http-address=0.0.0.0 --http-port=80 --docroot=.

WX_PREFIX=$($(which wx-config) --prefix)
export WX_PREFIX

if [[ "$OSTYPE" == "msys" ]]; then
boost_prefix=$PWD/ext/boost_1_82_0
echo "Using BOOST: $boost_prefix"
sleep 2
fi

mkdir -p ext/wt-4.10.0/build
pushd ext
pushd wt-4.10.0
pushd build

if [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DCMAKE_INSTALL_PREFIX=C:/wt_install -DBOOST_PREFIX="$boost_prefix" -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
cmake --install . --config Debug

elif [[ "$OSTYPE" == "darwin"* ]]; then

## <<<<<<< HEAD
## <<<<<<< HEAD
## cmake .. -DCMAKE_INSTALL_PREFIX=/Users/pvn/wt_install -DBOOST_PREFIX="$HOME/git/nostr_client_relay/ext/boost_1_82_0" -DINSTALL_EXAMPLES=ON
## cmake --build . --parallel 9
## make install
##
## elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
##
## cmake .. -DCMAKE_INSTALL_PREFIX="$HOME/wt_install" -DBOOST_PREFIX="$HOME/git/nostr_client_relay/ext/boost_1_82_0" \
## =======
## =======
## >>>>>>> e5f272ab (ext/wt-4.10.0/CMakeLists.txt: include_directories(../openssl-3.0.5))
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local -DBOOST_PREFIX="$PWD/ext/boost_1_82_0" -DINSTALL_EXAMPLES=ON
cmake --build . --parallel 9
make install

elif [[ "$OSTYPE" == "linux-gnu"* ]] || [[ "$OSTYPE" == "Darwin" ]]; then

cmake .. -DCMAKE_INSTALL_PREFIX="/usr/local" -DBOOST_PREFIX="$PWD/ext/boost_1_82_0" \
## <<<<<<< HEAD
## >>>>>>> 559c1748 (Initial commit)
## =======
## >>>>>>> e5f272ab (ext/wt-4.10.0/CMakeLists.txt: include_directories(../openssl-3.0.5))
-DBUILD_EXAMPLES=OFF -DENABLE_HARU=OFF -DENABLE_PANGO=OFF -DENABLE_POSTGRES=OFF -DENABLE_FIREBIRD=OFF -DENABLE_MYSQL=OFF -DENABLE_MSSQLSERVER=OFF \
-DENABLE_QT4=OFF -DENABLE_QT5=OFF -DENABLE_QT6=OFF -DENABLE_LIBWTTEST=OFF -DENABLE_LIBWTDBO=OFF -DENABLE_OPENGL=OFF
cmake --build . --parallel 1
make install

fi

popd
popd
popd



