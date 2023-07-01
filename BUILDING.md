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

## Options

Nostro either sends an EVENT or a REQ. Some command line options apply only to EVENT, others to REQ.

### Common options

*--uri* `<wss URI>`  
  WSS URI to send 
  
*--req*  
  message is a request (REQ). EVENT parameters are ignored 
  
*--sec* `<hex seckey>`  
  set the secret key for signing, otherwise one will be randomly generated
  
#### Generate pair private/public keys

Nostro can be used to generate the 2 sets of keys needed for the Nostr network (private/public keys). If no --sec parameter
is set, the generated private key is dumped to standard output and can be stored for later use.
  
### REQ options

*--id* `<hex>`  
  event id (hex) to look up on the request
  
*--rand*  
  send a RAND request (e.g ["REQ","RAND",{"kinds": [1], "limit": 2}])
  
### EVENT options

*--content* `<string>`     
  The text contents of the note

*--dm* `<hex pubkey>`      
  Create a direct message. This will create a kind-4 note with the contents encrypted

*--kind* `<number>`     
  Set the kind of the note

*--created-at* `<unix timestamp>`      
  Set the created at. Optional, this is set automatically.

*--mine-pubkey*  
  Mine a pubkey. This may or may not be cryptographically dubious.

*--pow* `<difficulty>`    
  Number of leading 0 bits of the id the mine for proof-of-work.

*--tag* `<key value>`      
  Add a tag with a single value

*-t*
  Shorthand for --tag t `<hashtag>`     

*-p*
  Shorthand for --tag p `<hex pubkey>`      

*-e*
  Shorthand for --tag e `<note id>`    

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

