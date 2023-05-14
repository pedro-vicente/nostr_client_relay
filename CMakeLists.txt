cmake_minimum_required(VERSION 3.24)
set(CMAKE_BUILD_TYPE Debug)
project (nostr_client_relay DESCRIPTION "Nostr client and relay demo" LANGUAGES CXX VERSION 1.0)

include_directories(ext/asio-1.18.1/include)
include_directories(ext/json-3.11.2/single_include)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_ADDRESSOF)
add_definitions(-DASIO_HAS_STD_ARRAY)
add_definitions(-DASIO_HAS_CSTDINT)
add_definitions(-DASIO_HAS_STD_SHARED_PTR)
add_definitions(-DASIO_HAS_STD_TYPE_TRAITS)
add_definitions(-DASIO_HAS_VARIADIC_TEMPLATES)
add_definitions(-DASIO_HAS_STD_FUNCTION)
add_definitions(-DASIO_HAS_STD_CHRONO)
add_definitions(-DBOOST_ALL_NO_LIB)
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

#//////////////////////////
#sources 
#//////////////////////////

set(src ${src})
set(src ${src} src/log.hh)
set(src ${src} src/log.cc)
set(src ${src} src/http.hh)
set(src ${src} src/http.cc)

#//////////////////////////
#executables 
#//////////////////////////

add_executable(client ${src} src/client.cc)
add_executable(relay ${src} src/relay.cc)

#//////////////////////////
#link
#//////////////////////////

if (MSVC)
  target_link_libraries (client Ws2_32.lib)
  target_link_libraries (relay Ws2_32.lib)
endif()
