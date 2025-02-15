# This file is part of 'Nostr_client_relay'
# Copyright (c) 2025, Space Research Software LLC, Pedro Vicente. All rights reserved.
# See file LICENSE for full license details.

if (MSVC)
 cmake_minimum_required(VERSION 3.26)
else()
 cmake_minimum_required(VERSION 3.22)
endif()
set(CMAKE_BUILD_TYPE Debug)
project (nostr_client_relay LANGUAGES CXX C)

if(MSVC)
  option(STATIC_MSVC "STATIC_MSVC" OFF)
endif()
option(BUILD_DESKTOP "BUILD_DESKTOP" OFF)
option(BUILD_WEB "BUILD_WEB" OFF)
option(BUILD_MOBILE "BUILD_MOBILE" OFF)
option(USE_OPENSSL "set OFF to build without OpenSSL" ON)
option(USE_BUILT_OPENSSL "Use OpenSSL built from source (Windows)" OFF)
option(DEV_MODE "DEV_MODE" OFF)

if(MSVC)
if(STATIC_MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
endif()

if(MSVC)
  message(STATUS "Windows static build: " ${STATIC_MSVC})
endif()  
message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

#//////////////////////////
# external dependencies
#//////////////////////////

include_directories(${CMAKE_SOURCE_DIR}/ext/asio-1.18.1/include)
include_directories(${CMAKE_SOURCE_DIR}/ext/json-3.11.2/single_include)
include_directories(${CMAKE_SOURCE_DIR}/ext/secp256k1-0.3.2/include)
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/secp256k1-0.3.2)

if(BUILD_WEB)
if(MSVC)
if(DEV_MODE)
  set(BOOST_PREFIX ${CMAKE_SOURCE_DIR}/ext/boost_1_82_0)
  add_subdirectory(${CMAKE_SOURCE_DIR}/ext/wt-4.10.0)
  add_definitions(-DDEV_MODE)
endif()
endif()
endif()

#//////////////////////////
# nostril
#//////////////////////////

add_subdirectory(${CMAKE_SOURCE_DIR}/src/nostril)
include_directories(${CMAKE_SOURCE_DIR}/src/nostril)

include_directories(src)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#//////////////////////////
# Asio definitions
#//////////////////////////

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
# Linux/Mac
#//////////////////////////

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-deprecated -Wno-deprecated-declarations -Wno-nonnull")
  if (APPLE_IOS_TARGET)
    message(STATUS "iOS build")
  else()
    message(STATUS "Mac build")
  endif()
endif()

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-deprecated -Wno-deprecated-declarations -Wno-nonnull -Wno-psabi")
  message(STATUS "Linux build")
endif()

message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

#//////////////////////////
# Nostr library
#//////////////////////////

set(src ${src})
set(src ${src} src/log.hh)
set(src ${src} src/log.cc)
set(src ${src} src/nostr.hh)
set(src ${src} src/nostr.cc)
set(src ${src} src/relay.hh)
set(src ${src} src/relay.cc)
set(src ${src} src/uuid.cc)
set(src ${src} src/uuid.hh)
set(src ${src} src/database.hh)
set(src ${src} src/database.cc)

add_library(nostr ${src})
if (NOT BUILD_MOBILE)
add_dependencies(nostr configurator)
endif()

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})
set(lib_dep ${lib_dep} nostr)
set(lib_dep ${lib_dep} nostri)
if (MSVC)
  set(lib_dep ${lib_dep} Ws2_32.lib)
  set(lib_dep ${lib_dep} Bcrypt.lib)
endif()

#//////////////////////////
# LINUX
# order of the link libraries matters; pthread dl
#//////////////////////////

if(LINUX)
  set(lib_dep ${lib_dep} pthread dl)
  set(lib_dep ${lib_dep} stdc++fs)
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

#//////////////////////////
# OpenSSL
#//////////////////////////

if(USE_OPENSSL)
  include_directories(${CMAKE_SOURCE_DIR}/ext/Simple-WebSocket-Server-v2.0.2)
  add_definitions(-DUSE_STANDALONE_ASIO)
  set (ASIO_PATH ${CMAKE_SOURCE_DIR}/ext/asio-1.18.1)
endif()

if(USE_OPENSSL)
  if (MSVC)
    if(USE_BUILT_OPENSSL)
      set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/openssl-3.0.5")
      message(STATUS "Setting OPENSSL_ROOT_DIR to: ${OPENSSL_ROOT_DIR}")
     endif(USE_BUILT_OPENSSL)
  endif(MSVC)

 if (APPLE_IOS_TARGET)
   set(OPENSSL_ROOT_DIR "/Users/pvn/dev/openssl-apple/bin/iPhoneOS16.4-arm64.sdk")
   message(STATUS "iOS OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
   include_directories("/Users/pvn/dev/openssl-apple/bin/iPhoneOS16.4-arm64.sdk/include/openssl")
   link_directories("/Users/pvn/dev/openssl-apple/bin/iPhoneOS16.4-arm64.sdk/lib/")
 endif()

  find_package(OpenSSL REQUIRED)
  message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
  include_directories(${OPENSSL_INCLUDE_DIR})
  set(lib_dep ${lib_dep} ${OPENSSL_LIBRARIES})
  if (MSVC)
    set(lib_dep ${lib_dep} crypt32.lib)
  endif()
endif()

#//////////////////////////
# HTTP demo
#//////////////////////////

if (NOT BUILD_MOBILE)
  add_executable(http_client src/http/http.hh src/http/http.cc src/http/http_client.cc)
  add_executable(http_relay src/http/http.hh src/http/http.cc src/http/http_relay.cc)
  target_link_libraries (http_client ${lib_dep})
  target_link_libraries (http_relay ${lib_dep})
endif()

#//////////////////////////
# executables
#//////////////////////////

if (NOT BUILD_MOBILE)
  add_executable(nostro src/wss_client.cc)
  add_executable(vostro src/wss_relay.cc)
  target_link_libraries (nostro ${lib_dep})
  target_link_libraries (vostro ${lib_dep})
endif()

#//////////////////////////
# tests
#//////////////////////////

if (NOT BUILD_MOBILE)
  add_executable(tests test/tests.cc)
  target_link_libraries (tests ${lib_dep})
endif()

#//////////////////////////
# examples
#//////////////////////////

if (NOT BUILD_MOBILE)
  add_executable(examples examples/examples.cc)
  target_link_libraries (examples ${lib_dep})
endif()

#//////////////////////////
# IDE project virtual folders
#//////////////////////////

if (NOT BUILD_MOBILE)
  set_target_properties(http_client PROPERTIES FOLDER "http")
  set_target_properties(http_relay PROPERTIES FOLDER "http")
  set_target_properties(nostril PROPERTIES FOLDER "nostril")
  set_target_properties(configurator PROPERTIES FOLDER "nostril")
endif()
set_target_properties(secp256k1 PROPERTIES FOLDER "nostril")
set_target_properties(secp256k1_precomputed PROPERTIES FOLDER "nostril")
set_target_properties(nostri PROPERTIES FOLDER "nostril")


if(BUILD_WEB)
if(MSVC)
if(DEV_MODE)
  set_target_properties(wt PROPERTIES FOLDER "wt")
  set_target_properties(wthttp PROPERTIES FOLDER "wt")
  set_target_properties(wtisapi PROPERTIES FOLDER "wt")
endif()
endif()
endif()

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/test/list_01.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/test/list_02.txt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/resources/server.key" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/resources/server.crt" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/resources/database.json" DESTINATION ${CMAKE_BINARY_DIR})

#//////////////////////////
# Wt desktop
#//////////////////////////

set(HOME_DIR $ENV{HOME})

if (BUILD_DESKTOP)
  file(COPY "${CMAKE_SOURCE_DIR}/resources/feed.json" DESTINATION ${HOME_DIR}/nostro)
  add_subdirectory(desktop)
endif()

#//////////////////////////
# Wt web
#//////////////////////////

if (BUILD_WEB)
 add_subdirectory(web)
endif()

#//////////////////////////
# iOS, Android
#//////////////////////////

if (BUILD_MOBILE)
 add_subdirectory(mobile)
endif()

