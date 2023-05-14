## <<<<<<< HEAD
## #!/bin/bash
##
## with_gui="$1"
## if [ -z "$with_gui" ]
## then
##  echo "Building without GUI"
##  opt=OFF
## else
##  echo "Building with GUI"
##  opt=ON
## fi
## =======
#!/usr/bin/env bash

[ -z "${p}" ] && port='6102' || port=${p}

VOSTRO=$(lsof -i -n -P | grep "$port" | grep vostro)
if [[ "$VOSTRO" == *"vostro"* ]]; then
	echo "vostro already using port:$PORT" && exit;
fi
##absolute path of script
SCRIPT=$(realpath -s "$0")
##directory script is in
SCRIPTPATH=$(dirname "$SCRIPT")
SCRIPT_PATH="${BASH_SOURCE}"
#echo $SCRIPT
echo $SCRIPTPATH
#echo $SCRIP_TPATH

WX_PREFIX=$($(which wx-config) --prefix)
export WX_PREFIX

sleep 3

if [[ "$OSTYPE" == "msys" ]]; then
dir=$PWD
fi

mkdir -p build
pushd build
## <<<<<<< HEAD
## if [[ "$OSTYPE" == "linux-gnu"* ]]; then
##
## cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" -DBUILD_WEB=$opt -DBUILD_GUI=$opt
##
## elif [[ "$OSTYPE" == "darwin"* ]]; then
##
## cmake .. -DWT_INCLUDE="$HOME/wt_install/include" -DWT_CONFIG_H="$HOME/wt_install/include" -DBUILD_WEB=$opt -DBUILD_GUI=$opt
## =======

if [[ "$OSTYPE" == "linux-gnu"* ]]; then

cmake .. -DWT_INCLUDE="${WX_PREFIX}/lib/include" -DWT_CONFIG_H="${WX_PREFIX}/include" -DBUILD_WEB=$opt -DBUILD_GUI=$opt

elif [[ "$OSTYPE" == "darwin"* ]]; then

cmake .. -DWT_INCLUDE="${WX_PREFIX}/lib/include" -DWT_CONFIG_H="${WX_PREFIX}/include" -DBUILD_WEB=$opt -DBUILD_GUI=$opt

elif [[ "$OSTYPE" == "msys" ]]; then

cmake .. --fresh -DBUILD_STATIC=OFF -DWT_INCLUDE="$dir/ext/wt-4.10.0/src" -DWT_CONFIG_H="$dir/ext/wt-4.10.0/build" \
-DBUILD_WEB=$opt -DBUILD_GUI=$opt
cmake --build .

fi

## <<<<<<< HEAD
## =======
## with_gui="$1"
## if [ -z "$with_gui" ]
## then
##  echo "Building without GUI"
##  opt=OFF
## else
##  echo "Building with GUI"
##  opt=ON
## fi
## >>>>>>> 559c1748 (Initial commit)
sleep 3
cmake --build .

popd
## <<<<<<< HEAD
## pwd
## =======
echo `pwd`

pushd build
pushd web

remote=$(git config --get remote.origin.url)
echo "remote repository: $remote"
sleep 2
if [ "$remote" == "https://github.com/pedro-vicente/nostr_client_relay.git" ]; then
## <<<<<<< HEAD
## export LD_LIBRARY_PATH="$HOME/github/nostr_client_relay/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
## else
## export LD_LIBRARY_PATH="$HOME/git/nostr_client_relay/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
## fi
## if [[ "$OSTYPE" == "msys"* ]]; then
## ./Debug/wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
## else
## ./wostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
## =======
	export LD_LIBRARY_PATH="$HOME/github/nostr_client_relay/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
else
	export LD_LIBRARY_PATH="$SCRIPTPATH/ext/boost_1_82_0/stage/lib":$LD_LIBRARY_PATH
fi
if [[ "$OSTYPE" == "msys"* ]]; then
	./Debug/vostro --http-address=0.0.0.0 --http-port=8080  --docroot=.
else
	pushd $SCRIPTPATH/build && ./vostro $port || echo "port busy?"
fi

exit

