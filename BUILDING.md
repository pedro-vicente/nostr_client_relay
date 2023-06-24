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

## Building

The CMake build utility is used. There are 3 modes of build. Default is command line only. To enable desktop and web builds, define at command line 

```
cmake -DBUILD_WEB=ON -DBUILD_GUI=ON

```

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

``` cmd
./build.cmake.sh
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

