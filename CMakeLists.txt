cmake_minimum_required(VERSION 3.15)
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
#Linux/Mac
#//////////////////////////

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  message(STATUS "Mac OS build: ${CMAKE_CXX_FLAGS}")
endif()

#//////////////////////////
#link with libraries
#lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
#LINUX
#order of the link libraries matters; pthread dl ${ODBC_LIBRARY} must be last
#//////////////////////////

if(LINUX)
  set(lib_dep ${lib_dep} pthread dl)
  set(lib_dep ${lib_dep} stdc++fs) #gcc 8.3.0
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

if (MSVC)
  set(lib_dep ${lib_dep} Ws2_32.lib)
endif()

#//////////////////////////
#sources 
#//////////////////////////

set(src ${src})
set(src ${src} src/log.hh)
set(src ${src} src/log.cc)
set(src ${src} src/http.hh)
set(src ${src} src/http.cc)
set(src ${src} src/message.hh)
set(src ${src} src/message.cc)
set(src ${src} src/database.hh)
set(src ${src} src/database.cc)

#//////////////////////////
#executables 
#//////////////////////////

add_executable(client ${src} src/client.cc)
add_executable(relay ${src} src/relay.cc)

#//////////////////////////
#link
#//////////////////////////

target_link_libraries (client ${lib_dep})
target_link_libraries (relay ${lib_dep})

