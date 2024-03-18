# Dependencies

- CMake [https://cmake.org/]
- libsecp256k1 [https://github.com/bitcoin-core/secp256k1]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)
- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- Simple-WebSocket-Server [https://gitlab.com/eidheim/Simple-WebSocket-Server] 
- JSON Modern C++ [https://json.nlohmann.me/] 

Nostr_client_relay allows to build Nostr clients and Nostr relays. It includes:

- a command line Nostr client, **Nostro**
- a command line Nostr relay, **Vostro**
- a native desktop client, **Gnostro** (for Mac, Linux, Windows)
- a web interface for a client, **Wostro**
- a native mobile client, **Mostro** (deprecated)

:warning: All dependencies except boost (for web build) and wxWidgets (for desktop build) are included in the repository.  
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

For desktop, the additional library is needed  

- WxWidgets [https://www.wxwidgets.org/]

To clone WxWidgets, do

```
git clone --recurse-submodules https://github.com/wxWidgets/wxWidgets.git ext/wxWidgets-3.2.2.1
```

build dependencies with the bash shell script

``` cmd
build.widgets.sh
```

To enable desktop build, use 

```
cmake -DBUILD_DESKTOP=ON

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





