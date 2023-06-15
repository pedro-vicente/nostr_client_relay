## Nostr client and relay

Nostr client and relay is a Nostr [https://nostr.com/]  C++ engine that allows to build Nostr applications for command line, desktop or web.

https://github.com/pedro-vicente/nostr_client_relay

A modified version of Nostril [https://github.com/jb55/nostril] is used for event and request generation. All events or requests are enveloped by default (ready to send to the wire)

**ATTENTION DEVELOPERS**

See [BUILDING.md](./BUILDING.md) for source code build instructions. There are 3 non exclusive modes of build (command line, desktop and web). Default is command line only.

## Nostro and Vostro

Nostro and Vostro are command line client and relay programs. Start a shell with 'nostro' and another shell with 'vostro'. Vostro is currently an echo server. Nostro allows to connect to Vostro on localhost (no --uri parameter) or to any publicly available Nostr relay. 

## Options

Nostro either sends an EVENT or a REQ. Some command line options apply only to EVENT, others to REQ.

### Common options

*--uri* <wss URI>
  WSS URI to send 
  
*--req* 
  message is a request (REQ). EVENT parameters are ignored 
  
*--sec* <hex seckey>
  set the secret key for signing, otherwise one will be randomly generated
  
### REQ options

*--id* <hex>
  event id (hex) to look up on the request; if none a random id is sent
  
*--rand*
  send a RAND request (e.g ["REQ","RAND",{"kinds": [1], "limit": 2}])
  
### EVENT options

*--content* <string>
  The text contents of the note

*--dm* <hex pubkey>
  Create a direct message. This will create a kind-4 note with the contents encrypted>

*--kind* <number>
  Set the kind of the note

*--created-at* <unix timestamp>
  Set the created at. Optional, this is set automatically.

*--mine-pubkey*
  Mine a pubkey. This may or may not be cryptographically dubious.

*--pow* <difficulty>
  Number of leading 0 bits of the id the mine for proof-of-work.

*--tag* <key> <value>
  Add a tag with a single value

*-t*
  Shorthand for --tag t <hashtag>

*-p*
  Shorthand for --tag p <hex pubkey>

*-e*
  Shorthand for --tag e <note id>

## Usage

### Requests

Send a REQ with a RAND subscription 

```
./nostro --uri relay.damus.io --req --rand
```

``` json
["REQ",
"RAND",
{"kinds": [1], "limit": 2}]
```

Send a REQ for events with event id (if no id, a random id is generated) 

```
./nostro --uri relay.snort.social --req --id 92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363
```

``` json
["REQ",
"subscription_nostro",
{"kinds": [1],"ids": ["92cae1df88a32fe9ffa43cf81219404039125b155458885dd083af06b4bd3363"]}]
```

### Events

Send an EVENT with content and a private key (if no key, one is generated) 

```
./nostro --sec <key> --content "hello"
```

``` json
["EVENT",
{
    "id": "ed378d3fdda785c091e9311c6e6eeb075db349a163c5e38de95946f6013a8001",
    "pubkey": "fd3fdb0d0d8d6f9a7667b53211de8ae3c5246b79bdaf64ebac849d5148b5615f",
    "created_at": 1649948103,
    "kind": 1,
    "tags": [],
    "content": "hello",
    "sig": "9d9a49bbc66d4782030b24c71416965e790214d02a54ab132d960c2b02def0371c3d93e5a60a285c55e99721599d1332450731e2c6bb1114b96b591c6967f872"]
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

