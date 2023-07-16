## Dependencies

- CMake [https://cmake.org/]
- libsecp256k1 [https://github.com/bitcoin-core/secp256k1]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)
- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- Simple-WebSocket-Server [https://gitlab.com/eidheim/Simple-WebSocket-Server] 
- JSON Modern C++ [https://json.nlohmann.me/] 

### For Web build

- Boost [https://www.boost.org/]
- Wt [https://www.webtoolkit.eu/wt]

### For GUI desktop build

- WxWidgets [https://www.wxwidgets.org/]

## Building

The CMake build utility is used. There are 3 modes of build. Default is command line only. To build with defaults, use
on a shell

```
cmake -S . -B build
cd build
cmake --build . 

```

This builds the software on folder named 'build'. There you can find the executables and your code editor of choice
generated projects, like projects for Microsof Visual Studio (in Windows).

## Building for web and desktop

To enable desktop and web builds, define at command line 

```
cmake -DBUILD_WEB=ON -DBUILD_GUI=ON

```

but these require extra parameters to locate dependencies. Use the bash script to build and see examples 

``` cmd
./build.cmake.sh
```

## Install dependencies

To install dependencies on Mac and Linux

Mac

``` cmd 
brew install cmake
brew install openssl
```

Linux

``` cmd 
sudo apt-get install cmake
sudo apt-get install build-essential libgtk-3-dev 
sudo apt-get install libssl-dev 
```

For web, dependencies boost and wt are needed 

``` cmd
./build.boost.sh
./build.wt.sh
```


### Boost

Note: All dependencies except boost are included. Script ./build.boost.sh does a git clone of the boost repository, according to

https://github.com/boostorg/wiki/wiki/Getting-Started%3A-Overview

with 

```
git clone -b boost-1.82.0 --recursive --depth=1 https://github.com/boostorg/boost.git ext/boost_1_82_0
```

to run the script a second time, folder ext/boost_1_82_0 must be emptied with

```
cd ext/boost_1_82_0
find . -name . -o -prune -exec rm -rf -- {} +
```

