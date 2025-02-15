# Dependencies for command line applications 

- CMake [https://cmake.org/]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)

## Software included

These libraries are included in /ext

- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- Simple-WebSocket-Server [https://gitlab.com/eidheim/Simple-WebSocket-Server] 
- JSON Modern C++ [https://json.nlohmann.me/] 

## Dependencies for desktop GUI Nostro

GUI Nostro requires the Qt cross-platform application development framework. It can be installed with

Linux

``` cmd 
sudo apt install qt6-base-dev qt6-multimedia-dev
```

Windows
Add Qt location to PATH environment variable 
e.g.
C:\Qt\6.8.2\msvc2022_64


Nostr_client_relay allows to build Nostr clients and Nostr relays. It includes:

- a command line Nostr client, **Nostro**
- a command line Nostr relay, **Vostro**
- a native desktop client, **Gnostro** (for Mac, Linux, Windows)
- a web interface for a client, **Wostro**
- a native mobile client, **Mostro** (deprecated)

:warning: All dependencies except boost (for web build) and Qt (for desktop build) are included in the repository.  
The minimal dependency needed is OpenSSL. To install dependencies on Mac and Linux

Mac

``` cmd 
brew install cmake
brew install openssl@3.0
brew link openssl@3.0
```

Linux

``` cmd 
sudo apt-get install cmake
sudo apt-get install build-essential libgtk-3-dev 
sudo apt-get install libssl-dev 
```

on Windows , use the supplied script ext/openssl-3.0.5/build.bat

``` cmd 
cd ext/openssl-3.0.5
build.bat
```

# Quick start

To clone a copy with basic build do at a shell prompt

```
 git clone  https://github.com/pedro-vicente/nostr_client_relay
```

The CMake build utility is used. There are 4 modes of build.

- command line (default)
- web 
- desktop 
- mobile 

Default is command line only. To build the source code, use on a shell

```
cd nostr_client_relay
cmake -S . -B build
cd build
cmake --build . 

```

or use the supplied script 

``` cmd
build.cmake.sh
```

This builds the software on folder named 'build'. There you can find the executables and your code editor of choice
generated projects, like projects for Microsof Visual Studio (in Windows).

Try

``` cmd
cd build
./examples
```

To run the supplied Nostr command line API examples

# Desktop build

For desktop, install dependencies 

Linux Ubuntu  

```
sudo apt-get install git cmake build-essential libgl1-mesa-dev libssl-dev
```


To enable desktop build, use 

```
cmake .. -DBUILD_DESKTOP=ON -DCMAKE_PREFIX_PATH="/home/pvn/Qt-6.7.0

```

or build with bash shell script

``` cmd
./build.desktop.sh 
```

# Web build

Dependencies boost, Wt, are needed for web build. 

- Boost [https://www.boost.org/]
- Wt [https://www.webtoolkit.eu/wt]

To enable web build, use 

```
cmake -DBUILD_WEB=ON

```
build dependencies with bash shell scripts

``` cmd
./build.boost.sh
./build.wt.sh
```

Use the bash script to build 

``` cmd
./build.web.sh 
```

To clone Boost individually, do

```
git clone -b boost-1.82.0 --recursive --depth=1 https://github.com/boostorg/boost.git ext/boost_1_82_0
```





