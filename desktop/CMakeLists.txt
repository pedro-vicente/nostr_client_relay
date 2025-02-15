cmake_minimum_required(VERSION 3.16)
project(gnostro VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Multimedia MultimediaWidgets)
qt_standard_project_setup()

#//////////////////////////
# list of source files 
#//////////////////////////

include_directories("${CMAKE_SOURCE_DIR}/src")

set(src_gui ${src_gui})
set(src_gui ${src_gui} main.cc)
set(src_gui ${src_gui} nostro.h)
set(src_gui ${src_gui} window.cc)
set(src_gui ${src_gui} window.h)
set(src_gui ${src_gui} entry.cc)
set(src_gui ${src_gui} entry.h)
set(src_gui ${src_gui} store.cc)
set(src_gui ${src_gui} store.h)
set(src_gui ${src_gui} feed.cc)
set(src_gui ${src_gui} feed.h)
set(src_gui ${src_gui} twitter.h)

add_executable(gnostro ${src_gui})
target_link_libraries(gnostro PRIVATE Qt6::Widgets)
target_link_libraries(gnostro PRIVATE Qt6::Network)
target_link_libraries(gnostro PRIVATE Qt6::Multimedia)
target_link_libraries(gnostro PRIVATE Qt6::MultimediaWidgets)

#//////////////////////////
# link with (imported from root)
#//////////////////////////

target_link_libraries(gnostro PRIVATE ${lib_dep})
message(STATUS "Link with: " ${lib_dep})

set_target_properties(gnostro PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
