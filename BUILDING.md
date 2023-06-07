## Building

The CMake build utility is used. There are 3 modes of build. Default is command line only. To enable desktop and web builds, define at command line 

```
-DBUILD_WEB=ON -DBUILD_GUI=ON

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
chmod +x build.cmake.sh
./build.cmake.sh
```

For web, dependencies boost and wt are needed 

``` cmd
chmod +x build.boost.sh
chmod +x build.wt.sh
./build.boost.sh
./build.wt.sh
./build.cmake.sh
```

### Boost

Note: All dependencies except boost are included. Script ./build.boost.sh does a git clone of the boost repository, according to

https://github.com/boostorg/wiki/wiki/Getting-Started%3A-Overview

with 

git clone --recursive https://github.com/boostorg/boost.git ext/boost_1_82_0

to run the script a second time, folder ext/boost_1_82_0 must be emptied with

```
cd ext/boost_1_82_0
find . -name . -o -prune -exec rm -rf -- {} +
```

file .gitmodules lists all boost submodules (in relative path) 
