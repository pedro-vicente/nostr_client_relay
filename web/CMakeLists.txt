message(STATUS "WT_INCLUDE: " ${WT_INCLUDE})
message(STATUS "WT_CONFIG_H: " ${WT_CONFIG_H})
message(STATUS "WT_ROOT: " ${WT_ROOT})

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

include_directories(${WT_INCLUDE})
include_directories(${WT_CONFIG_H})

add_executable(nostro_web nostro_web.cc)

#//////////////////////////
#link with libraries
#LIB_DEP contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_web ${lib_web})

if (MSVC)
  set(lib_web ${lib_web} N:/nostr_client_relay/ext/wt-4.10.0/build/src/Debug/wtd.lib)
  set(lib_web ${lib_web} N:/nostr_client_relay/ext/wt-4.10.0/build/src/http/Debug/wthttpd.lib)
  set (BOOST_LIB_DIRS N:/nostr_client_relay/ext/boost_1_82_0/lib/)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem-vc143-mt-gd-x64-1_82.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread-vc143-mt-gd-x64-1_82.lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options-vc143-mt-gd-x64-1_82.lib)
endif()

if(APPLE)
  set(lib_web ${lib_web} /Users/pvn/wt_install/lib/libwt.dylib)
  set(lib_web ${lib_web} /Users/pvn/wt_install/lib/libwthttp.dylib) 
  set(BOOST_LIB_DIRS /Users/pvn/git/nostr_client_relay/ext/boost_1_82_0/stage/lib)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread.a)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options.a)
  
endif()

if (LINUX)
  if(WT_ROOT)
    set(lib_web ${lib_web} /root/wt_install/lib/libwt.so)
    set(lib_web ${lib_web} /root/wt_install/lib/libwthttp.so)
    set(BOOST_LIB_DIRS /root/git/nostr_client_relay/ext/boost_1_82_0/stage/lib)
  else()
    set(lib_web ${lib_web} /home/pvn/wt_install/lib/libwt.so)
    set(lib_web ${lib_web} /home/pvn/wt_install/lib/libwthttp.so)
    set(BOOST_LIB_DIRS /home/pvn/git/nostr_client_relay/ext/boost_1_82_0/stage/lib)
  endif()

  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_filesystem.so)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_thread.so)
  set(lib_web ${lib_web} ${BOOST_LIB_DIRS}/libboost_program_options.so)
  
endif()

set(lib_web ${lib_web} ${lib_dep})
target_link_libraries (nostro_web ${lib_web})


message(STATUS "Copying data files to: ${CMAKE_BINARY_DIR}/web")
file(COPY "${CMAKE_SOURCE_DIR}/nostro.css" DESTINATION ${CMAKE_BINARY_DIR}/web)
add_custom_target(css SOURCES ${CMAKE_BINARY_DIR}/web/nostro.css)
