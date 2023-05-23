#!/bin/bash
mkdir -p build
pushd build
cmake .. --fresh -DUSE_STANDALONE_ASIO=ON -DASIO_PATH=k:/nostr_client_relay/ext/asio-1.18.1/include -DOPENSSL_ROOT_DIR=k:/nostr_client_relay/ext/openssl-3.0.5 \
 -DOPENSSL_INCLUDE_DIR=k:/nostr_client_relay/ext/openssl-3.0.5/include 
cmake --build . 
popd
exit


