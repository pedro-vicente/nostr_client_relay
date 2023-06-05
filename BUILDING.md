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
brew install boost
```

Linux

``` cmd 
sudo apt-get install cmake
sudo apt-get install build-essential libgtk-3-dev 
sudo apt-get install libssl-dev 
sudo apt-get install libboost-all-dev
```

``` cmd
./build.cmake.sh
```

For web

``` cmd
./build.wt.sh
./build.cmake.sh
```