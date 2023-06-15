## Nostr client and relay

Nostr client and relay is a Nostr [https://nostr.com/]  C++ engine that allows to build Nostr applications for command line, desktop or web.

https://github.com/pedro-vicente/nostr_client_relay

A modified version of Nostril [https://github.com/jb55/nostril] is used for event and request generation. All events or requests are enveloped by default (ready to send to the wire)

**ATTENTION DEVELOPERS**

See [BUILDING.md](./BUILDING.md) for source code build instructions. There are 3 non exclusive modes of build (command line, desktop and web). Default is command line only.

## Nostro and Vostro

Nostro and Vostro are command line client and relay programs. Start a shell with 'nostro' and another shell with 'vostro'. Vostro is currently an echo server. Nostro allows to connect to Vostro on localhost (no --uri parameter) or to any publicly available Nostr relay. 

## Usage

### Requests

Send a REQ with a RAND subscription 

```
./nostro --uri relay.damus.io --req --rand

["REQ","RAND",{"kinds": [1], "limit": 2}]
```

Send a REQ for events with event id (if no id, a random id is generated) 

```
./nostro --uri relay.snort.social --req --id 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363

["REQ","subscription_nostro",{"kinds": [1],"ids": "92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363"}]
```

### Events

Send an EVENT with content and a public key (if no key, one is generated) 

```
./nostro --content hello --sec 7f612229528369d91ddcaae527f097ab4c7cacd0058fa46d5857f74f88ad1a5e 

["EVENT",{"id": "a5cd029dfe3098fd2241b92a01162b5a0d398b7c5af5f16d1d9ef2efa1326369","pubkey": "f8d5b589fc116b7171fb0c5d4555b1c27ff4860a6cfbcb4cf2412583312b5a29","created_at": 1686036353,"kind": 1,"tags": [],"content": "hello","sig": "43c5c7b1fdc9077ec4bdc5beb3c4346188a86d42f5eab879b890d57b82a65bfb365044fc0f3a750c9d07250e4151dd346fa1d11d2599a5739483aeebcc213c75"}]

```

### Dependencies

- CMake [https://cmake.org/]
- libsecp256k1 [https://github.com/bitcoin-core/secp256k1]
- OpenSSL [https://www.openssl.org/] (Windows build with ext/openssl-3.0.5/build.bat)
- Asio [https://think-async.com/Asio/AsioStandalone.html] 
- Simple-WebSocket-Server [https://gitlab.com/eidheim/Simple-WebSocket-Server] 
- JSON Modern C++ [https://json.nlohmann.me/] 

#### For Web build

- Boost [https://www.boost.org/]
- Wt [https://www.webtoolkit.eu/wt]

### Running on web

At command line

```
/nostro_web --http-address=0.0.0.0 --http-port=8080 --docroot=.
```

Open a browser at localhost port 8080

```
http://127.0.0.1/8080
```

![Nostro on the web](https://pedro-vicente.net/images/nostro.png)

