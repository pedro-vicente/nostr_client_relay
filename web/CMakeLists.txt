cmake_minimum_required(VERSION 3.1)
project (nostro_web)

message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "WT_CONFIG_H: " ${WT_CONFIG_H})
message(STATUS "WT_ROOT: " ${WT_ROOT})

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${WT_INCLUDE})
include_directories(${WT_CONFIG_H})

set(web ${web} )
set(web ${web} nostro_web.cc)

add_executable(nostro_web ${web})

#//////////////////////////
#link with libraries
#LIB_DEP contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(LIB_WEB ${LIB_WEB})

if (MSVC)
  set(LIB_WEB ${LIB_WEB} N:/nostr_client_relay/ext/wt-4.10.0/build/src/Debug/wtd.lib)
  set(LIB_WEB ${LIB_WEB} N:/nostr_client_relay/ext/wt-4.10.0/build/src/http/Debug/wthttpd.lib)
  set (BOOST_LIB_DIRS N:/nostro_web/boost_1_82_0/lib/)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_82.lib)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_82.lib)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_82.lib)
endif()

#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/Users/pvn/git/nostro_web/boost_1_82_0
#The following directory should be added to linker library paths:
#/Users/pvn/git/nostro_web/boost_1_82_0/stage/lib

#The Boost C++ Libraries were successfully built!
#The following directory should be added to compiler include paths:
#/home/pvn/git/nostro_web/boost_1_82_0
#The following directory should be added to linker library paths:
#/home/pvn/git/nostro_web/boost_1_82_0/stage/lib

if(APPLE)
  set(LIB_WEB ${LIB_WEB} /Users/pvn/wt_install/lib/libwt.dylib)
  set(LIB_WEB ${LIB_WEB} /Users/pvn/wt_install/lib/libwthttp.dylib) 
  set (BOOST_LIB_DIRS /Users/pvn/git/nostro_web/boost_1_82_0/stage/lib)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_filesystem.a)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_thread.a)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_program_options.a)
  
endif()

if (LINUX)
  if(WT_ROOT)
    set(LIB_WEB ${LIB_WEB} /root/wt_install/lib/libwt.so)
    set(LIB_WEB ${LIB_WEB} /root/wt_install/lib/libwthttp.so)
    set (BOOST_LIB_DIRS /root/git/nostro_web/boost_1_82_0/stage/lib)
  else()
    set(LIB_WEB ${LIB_WEB} /home/pvn/wt_install/lib/libwt.so)
    set(LIB_WEB ${LIB_WEB} /home/pvn/wt_install/lib/libwthttp.so)
    set (BOOST_LIB_DIRS /home/pvn/git/nostro_web/boost_1_82_0/stage/lib)
  endif()

  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_filesystem.so)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_thread.so)
  set(LIB_WEB ${LIB_WEB} ${BOOST_LIB_DIRS}/libboost_program_options.so)
  
endif()

set(LIB_WEB ${LIB_WEB} ${LIB_WT})
set(LIB_WEB ${LIB_WEB} ${lib_dep})
target_link_libraries (nostro_web ${LIB_WEB})
