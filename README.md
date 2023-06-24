## Nostr client and relay

Nostr client and relay is a Nostr [https://nostr.com/]  C++ engine that allows to build Nostr applications for command line, desktop or web.

https://github.com/pedro-vicente/nostr_client_relay

A modified version of Nostril [https://github.com/jb55/nostril] is used for event and request generation.

**ATTENTION DEVELOPERS**

See [BUILDING.md](./BUILDING.md) for source code build instructions. There are 3 non exclusive modes of build (command line, desktop and web). Default is command line only.

## Nostro and Vostro

Nostro and Vostro are command line client and relay programs. Start a shell with 'nostro' and another shell with 'vostro'. Vostro is currently an echo server. 
Nostro allows to connect to Vostro on localhost(localhost:8080/nostr) or to any publicly available Nostr relay. 

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

## Usage

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
### Web interface

The nostro web interface at this time allows input of a limited set of the command line options.

![Nostro on the web](https://pedro-vicente.net/images/nostro.png)

It is available at

[https://nostro.cloud/]


