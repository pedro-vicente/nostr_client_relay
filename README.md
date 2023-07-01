## Nostr client and relay

Nostr client and relay is a [https://nostr.com/](https://nostr.com/)  C++ engine that allows to build Nostr applications for command line, desktop or web.

[https://github.com/pedro-vicente/nostr_client_relay](https://github.com/pedro-vicente/nostr_client_relay)

A modified version of [Nostril](https://github.com/jb55/nostril) is used for event and request generation.

**ATTENTION DEVELOPERS**

See [BUILDING.md](./BUILDING.md) for source code build instructions. There are 3 non exclusive modes of build (command line, desktop and web). Default is command line only.

## Nostro and Vostro

Nostro and Vostro are command line client and relay programs. Start a shell with 'nostro' and another shell with 'vostro'. Vostro is currently an echo server. 
Nostro allows to connect to Vostro on localhost(localhost:8080/nostr) or to any publicly available Nostr relay. 

## Usage examples 

### Requests

Send a REQ with a RAND subscription 

```
./nostro --uri relay.damus.io --req --rand
```

``` json
[
 "REQ",
 "RAND",
 {
  "kinds": [
   1
  ],
  "limit": 5
 }
]
```

Send a REQ for event with event id

```
./nostro --req --id d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7
```

``` json
[
 "REQ",
 "subscription_nostro",
 {
  "ids": [
   "d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7"
  ],
  "kinds": [
   1
  ]
 }
]
```

A typical response is of the form

```json
["EVENT","subscription_nostro",{"id":"d75d56b2141b12be96421fc5c913092cda06904208ef798b51a28f1c906bbab7","kind":1,"pubkey":"4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b","created_at":1686811876,"content":"hello","tags":[],"sig":"4130b06203e3fc6d17c13208f5a0c80a58fb2c986aa417a2f71e7582dc70b918d4bb1b8226d5b36ae51897a3a04dc86cd7e54fedf3845858e29f8ce9f98e9647"}]
["EOSE","subscription_nostro"]
```


### Events

Send an EVENT with content and a private key (if no key, one is generated) 

```
./nostro --sec <key> --content "hello world"
```

``` json
[
 "EVENT",
 {
  "content": "hello world",
  "created_at": 1686881740,
  "id": "de0235b821a8f11e4baaf596cc4eac9690f35766daad891e0290407ffe0ba811",
  "kind": 1,
  "pubkey": "a9b6e9ad461890295a5739da81b85879b2e0e8d9062c595ab9d0dc3cc3a69f61",
  "sig": "d7280c90a60965dfe8722f04fd652cdc7dd617d292737fe43a27fc65ede240932489aae9226c3b699d0a3fe9203e2364e622d582a1d4ae38b1f4df966cd256dd",
  "tags": []
 }
]
```

### Running on web

At command line

```
./nostro_web --http-address=0.0.0.0 --http-port=8080 --docroot=.
```

Open a browser at localhost port 8080

```
http://127.0.0.1/8080
```
## Web interface

The nostro web interface at this time allows input of a limited set of the command line options.

![Nostro on the web](https://pedro-vicente.net/images/nostro.png)

It is available at

[https://nostro.cloud/]

## API

Nostr_client_relay allows an easy integration between C++ objects like strings and vectors and Nostr JSON entities like events and filters, 
defined in [NIP-01](https://github.com/nostr-protocol/nips/blob/master/01.md), using the [JSON for modern C++ library](https://github.com/nlohmann/json)

```cpp
std::string make_request(const std::string& subscription_id, const filter_t& filter);
```

```cpp
Type get_message_type(const std::string& json);
```

```cpp
int parse_event(const std::string& json, std::string& event_id, nostr::event_t& ev);
```

```cpp
int parse_request(const std::string& json, std::string& request_id, nostr::filter_t& filter);
```

```cpp
int relay_to(const std::string& uri, const std::string& json)
```



## Examples

### Get list of follows

To get a list of follows we start with a public key that we want to get the follows from and a relay address where the list is stored

```cpp
const std::string pubkey("4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b");
const std::string uri("nos.lol");
```

To define the request (REQ) we define a filter that has as "authors" our public key and has kind "3", defined as list of contacts in 
[NIP-02](https://github.com/nostr-protocol/nips/blob/master/02.md). A random and unique subscription ID for the request must also
ge generated, with:

```cpp
std::string subscription_id = uuid::generate_uuid_v4();
nostr::filter_t filter;
filter.authors.push_back(pubkey);
filter.kinds.push_back(3);
```

This generates the following JSON, optinally formatted for display with indentation 

```json
[
 "REQ",
 "9B874BC0-8372-4A41-9F5B-3DD6859F37F0",
 {
  "authors": [
   "4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b"
  ],
  "kinds": [
   3
  ],
  "limit": 1
 }
]
```

Finally the JSON string can be generated from the C++ objects and sent to the wire simply with

```cpp
std::string json = nostr::make_request(subscription_id, filter);
nostr::relay_to(uri, json);
```

And for that request a response is obtained according to <a href="https://github.com/nostr-protocol/nips/blob/master/02.md"> NIP-02 </a>


```json
[
 "EVENT",
 "5B695BFF-2A99-48AD-AAAE-4DF13F2DB7E4",
 {
  "content": "{\"wss://nos.lol/\":{\"write\":true,\"read\":true}}",
  "created_at": 1687486321,
  "id": "b8826c41b78bf8e4545706914e0d921b77a86192393ffb3df47f5623e6fa5b8f",
  "kind": 3,
  "pubkey": "4ea843d54a8fdab39aa45f61f19f3ff79cc19385370f6a272dda81fade0a052b",
  "sig": "b1d004f1c0f8e72c9ac0c4492086c4dfc60478156feb5d6e7075923bb6e0d738d69ed61d16d65503d6df1f09b363dccd8013bd56a5354ebbbb029f558363997e",
  "tags": [
   [
    "p",
    "c708943ea349519dcf56b2a5c138fd9ed064ad65ddecae6394eabd87a62f1770"
   ],
   [
    "p",
    "fa984bd7dbb282f07e16e7ae87b26a2a7b9b90b7246a44771f0cf5ae58018f52"
   ]
  ]
 }
]
```




