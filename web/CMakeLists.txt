# This file is part of 'Nostr_client_relay' 
# Copyright (c) 2025, Red Panda Software LLC, Pedro Vicente. All rights reserved.
# See file LICENSE for full license details.

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "BOOST_INCLUDE_DIR: " ${BOOST_INCLUDE_DIR})
message(STATUS "BOOST_LIB_DIRS: " ${BOOST_LIB_DIRS})

include_directories(${BOOST_INCLUDE_DIR})
include_directories(${WT_INCLUDE})
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

#//////////////////////////
# common sources library
#//////////////////////////

set(web ${web})
set(web ${web} feed.hh)
set(web ${web} feed.cc)
set(web ${web} home.hh)
set(web ${web} home.cc)
set(web ${web} follows.hh)
set(web ${web} follows.cc)

add_executable(wostro web.cc web.hh ${web})

#//////////////////////////
#link with libraries
#LIB_DEP contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_web ${lib_web})

#//////////////////////////
# Linux/Mac
#//////////////////////////

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-deprecated-declarations")
endif()

if(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-deprecated-copy -Wno-deprecated-copy-dtor")
endif()

message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

#//////////////////////////
# LINUX
# order of the link libraries matters; pthread dl
#//////////////////////////

if(LINUX)
  set(lib_web ${lib_web} pthread dl)
  set(lib_web ${lib_web} stdc++fs) 
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

if (MSVC)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/build/wt-4.12.0/src/Debug/wtd.lib)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/build/wt-4.12.0/src/http/Debug/wthttpd.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_88.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_88.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_88.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_chrono-vc143-mt-gd-x64-1_88.lib)
endif()

if(APPLE)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/install/wt/lib/libwt.dylib)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/install/wt/lib/libwthttp.dylib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem-clang-darwin17-mt-x64-1_88.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread-clang-darwin17-mt-x64-1_88.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options-clang-darwin17-mt-x64-1_88.a)
endif()

if (LINUX)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/install/wt/lib/libwt.so)
  set(lib_web ${lib_web} ${CMAKE_SOURCE_DIR}/install/wt/lib/libwthttp.so)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem-gcc13-mt-x64-1_88.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread-gcc13-mt-x64-1_88.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options-gcc13-mt-x64-1_88.a)
endif()

set(lib_web ${lib_web} ${lib_dep})
target_link_libraries (wostro ${lib_web})

message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}/web")
file(COPY "${CMAKE_SOURCE_DIR}/web/nostro.css" DESTINATION ${CMAKE_BINARY_DIR}/web)
add_custom_target(css SOURCES ${CMAKE_BINARY_DIR}/web/nostro.css)

if (MSVC)
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT wostro)
  set_property(TARGET wostro PROPERTY 
    VS_DEBUGGER_COMMAND_ARGUMENTS "--http-address=0.0.0.0 --http-port=8080  --docroot=.")
  set_property(TARGET wostro PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()
