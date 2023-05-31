## Nostr client and relay

Nostro and Vostro are Nostr [https://nostr.com/]  client and relay programs. 

### Test client and server

Start a shell with 'nostro' and another shell with 'vostro'. Vostro is currently an echo server. Nostro allows to connect to Vostro on localhost (no parameters) or to any publically available Nostr relay. 


```
./nostro -u relay.damus.io
nostro:14:47:49 Opened connection: HTTP 1.1 , code 101 Switching Protocols
nostro:14:47:49 Sending: "["REQ", "RAND", {"kinds": [1], "limit": 1}]"
nostro:14:47:49 Received: "["EVENT","RAND",{"content":"\nhttps://i.imgur.com/Zqo4RR8.jpg\n#Photography #photostr #blackwhite #plebchain #travel ","created_at":1684867664,"id":"837b42932e5c163a829de1a8b2e8b7234e0dca78b13a0bbeccd03b055fb6e2ef","kind":1,"pubkey":"f96c3d76497074c4c83a7b3823380e77dc73d5a9494fd2e053e4a1453e17824b","sig":"a3b1a74d9d6adc50451c513b30082cbbbef969bd08a9b65e972dc5daf15e9b869f56e39ce11477fe16ad8e4ab5666439be2e8abdd4ef73d58d87c424114bb63e","tags":[["t","Photography"],["t","photostr"],["t","blackwhite"],["t","plebchain"],["t","travel"],["r","https://i.imgur.com/Zqo4RR8.jpg"]]}]"


```

```
./nostro
vostro:14:51:26 Opened connection: 0x7f8bbcf06930
nostro:14:51:26 Opened connection: HTTP 1.1 , code 101 Web Socket Protocol Handshake
nostro:14:51:26 Sending: "["REQ", "RAND", {"kinds": [1], "limit": 3}]"
vostro:14:51:26 Server: Message received: "["REQ", "RAND", {"kinds": [1], "limit": 3}]" from 0x7f8bbcf06930
nostro:14:51:26 Received: "["REQ", "RAND", {"kinds": [1], "limit": 3}]"
vostro:14:51:26 Closed connection: 0x7f8bbcf06930 code 1000
nostro:14:51:26 Closed: 1000
nostro:14:51:26 Received 1 messages: 
nostro:14:51:26 ["REQ", "RAND", {"kinds": [1], "limit": 3}]

```

```
./vostro
vostro:14:54:13 Listening on port: 8080
vostro:14:54:27 Opened connection: 0x7ff052004080
vostro:14:54:27 Server: Message received: "["REQ", "RAND", {"kinds": [1], "limit": 3}]" from 0x7ff052004080
vostro:14:54:27 Closed connection: 0x7ff052004080 code 1000
```

### Dependencies

- CMake [https://cmake.org/]
- libsecp256k1 [https://github.com/bitcoin-core/secp256k1]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)
- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- Simple-WebSocket-Server [https://gitlab.com/eidheim/Simple-WebSocket-Server] 
- JSON Modern C++ [https://json.nlohmann.me/] 

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

### Building

``` cmd
./build.cmake.sh
```
