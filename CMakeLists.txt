# This file is part of 'Nostr_client_relay'
# Copyright (c) 2023, Space Research Software LLC, Pedro Vicente. All rights reserved.
# See file LICENSE for full license details.

if (MSVC)
 cmake_minimum_required(VERSION 3.26)
else()
 cmake_minimum_required(VERSION 3.15)
endif()
set(CMAKE_BUILD_TYPE Debug)
project (nostr_client_relay LANGUAGES CXX C)

option(BUILD_RELEASE "BUILD_RELEASE" ON)

if(BUILD_RELEASE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_STATIC "BUILD_STATIC" OFF)
option(BUILD_GUI "BUILD_STATIC" OFF)
option(BUILD_WEB "BUILD_WEB" OFF)
option(USE_OPENSSL "set OFF to build without OpenSSL" ON)
option(DEV_MODE "DEV_MODE" OFF)
option(OPENSSL_BREW "OPENSSL_BREW" ON)

if(MSVC)
if(BUILD_STATIC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
endif()

message("-- nostr_client_relay static build: " ${BUILD_STATIC})
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

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-deprecated -Wno-deprecated-declarations -Wno-nonnull -Wno-psabi")
  message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
endif()

if(UNIX AND APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-deprecated -Wno-deprecated-declarations -Wno-nonnull")
  message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")
endif()

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
add_dependencies(nostr configurator)

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})
set(lib_dep ${lib_dep} nostr)
set(lib_dep ${lib_dep} secp256k1)
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
#OpenSSL
#//////////////////////////

if(USE_OPENSSL)
  include_directories(${CMAKE_SOURCE_DIR}/ext/Simple-WebSocket-Server-v2.0.2)
  add_definitions(-DUSE_STANDALONE_ASIO)
  set (ASIO_PATH ${CMAKE_SOURCE_DIR}/ext/asio-1.18.1)
endif()

if(USE_OPENSSL)

	set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/openssl-3.0.5")
	find_package(OpenSSL REQUIRED)
	message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
	message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
	include_directories(${OPENSSL_INCLUDE_DIR})
	set(lib_dep ${lib_dep} ${OPENSSL_LIBRARIES})

  if (MSVC)
    set(OPENSSL_ROOT_DIR "${CMAKE_SOURCE_DIR}/ext/openssl-3.0.5")
    message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
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

add_executable(http_client src/http/http.hh src/http/http.cc src/http/http_client.cc)
add_executable(http_relay src/http/http.hh src/http/http.cc src/http/http_relay.cc)
target_link_libraries (http_client ${lib_dep})
target_link_libraries (http_relay ${lib_dep})

#//////////////////////////
# executables
#//////////////////////////

add_executable(gnostr-client src/wss_client.cc)
add_executable(gnostr-req src/wss_req.cc)
add_executable(gnostr-gnode src/wss_relay.cc)

target_link_libraries (gnostr-client ${lib_dep})
target_link_libraries (gnostr-req ${lib_dep})
target_link_libraries (gnostr-gnode ${lib_dep})

#//////////////////////////
# tests
#//////////////////////////

add_executable(tests test/tests.cc)
target_link_libraries (tests ${lib_dep})

#//////////////////////////
# examples
#//////////////////////////
if(UNIX AND NOT APPLE)
add_executable(examples examples/examples.cc)
target_link_libraries (examples ${l/ib_dep})
endif()

#//////////////////////////
# IDE project virtual folders
#//////////////////////////

set_target_properties(http_client PROPERTIES FOLDER "http")
set_target_properties(http_relay PROPERTIES FOLDER "http")
set_target_properties(nostri PROPERTIES FOLDER "nostril")
set_target_properties(nostril PROPERTIES FOLDER "nostril")
set_target_properties(configurator PROPERTIES FOLDER "nostril")
set_target_properties(secp256k1 PROPERTIES FOLDER "nostril")
set_target_properties(secp256k1_precomputed PROPERTIES FOLDER "nostril")

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
file(COPY "${CMAKE_SOURCE_DIR}/database/database.json" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/resources/wt_config.xml" DESTINATION ${CMAKE_BINARY_DIR}/web)

#//////////////////////////
# Wt desktop
#//////////////////////////

if (BUILD_GUI)
  add_subdirectory(desktop)
endif()

#//////////////////////////
# Wt web
#//////////////////////////

if (BUILD_WEB)
 add_subdirectory(web)
endif()


#//////////////////////////
# Install
#//////////////////////////

install(TARGETS gnostr-gnode
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/bin)
install(TARGETS gnostr-gnode
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)

install(TARGETS gnostr-client
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/debug)
install(TARGETS gnostr-client
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)

install(TARGETS gnostr-req
        CONFIGURATIONS Debug
        RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/debug)
install(TARGETS gnostr-req
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)


