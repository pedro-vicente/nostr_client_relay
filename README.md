## NOSTR client and relay demo


### Building

``` cmd
cmake -S . -B build
cd build 
cmake --build .
```

### Overview

A NOSTR client and relay demo. Currently transport is done with HTTP only (TCP) 

### Test

Start a shell with 'relay' and another shell with 'client'

```
./relay
relay:17:38:48 Listening on port:2000
relay:17:39:13 received POST from 127.0.0.1 466 bytes
relay:17:39:13 event received: some great content
relay:17:39:13 send response: 127.0.0.1
```
The client sends an EVENT and a REQ, and receives a response from the REQ

```
./client
client:17:39:13 sent 574 bytes
client:17:39:13 header: HTTP/1.1 200 OK
client:17:39:13 message: 
client:17:39:13 received empty message
client:17:39:13 sent 491 bytes
client:17:39:13 header: POST / HTTP/1.1
client:17:39:13 header: Host: 127.0.0.1
client:17:39:13 header: Connection: close
client:17:39:13 header: Content-Type: application/json
client:17:39:13 header: Content-Length: 455
client:17:39:13 message: [
  {
    "content": "some great content",
    "created_at": 1684186753,
    "id": "a3562c82dd4982a74ecf2d0294e22dcc",
    "kind": 1,
    "pubkey": "b2b41e936821bb48572c2295ec5f1741",
    "sig": "0132DB5A8941CE80C4FE1F1B4FD6C93DA6F8142AE6BA53A17909814C013CA694",
    "tags": [
      [
        "e",
        "a3562c82dd4982a74ecf2d0294e22dcc, 127.0.0.1"
      ],
      [
        "p",
        "b2b41e936821bb48572c2295ec5f1741, 127.0.0.1"
      ]
    ]
  }
]
client:17:39:13 received 1 events
client:17:39:13 some great content
```