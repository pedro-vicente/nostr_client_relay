message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
  set (CMAKE_PREFIX_PATH "F:\\qt6\\qt-everywhere-src-6.5.2\\qtbase\\bin")
endif() 

if(APPLE)
  if (APPLE_IOS_TARGET)
    set (CMAKE_PREFIX_PATH "/Users/pvn/Qt6.5.2/6.5.2/ios")
  else()
    set (CMAKE_PREFIX_PATH "/Users/pvn/Qt6.5.2/6.5.2/macos")
  endif()
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

add_executable(mostro window.h window.cpp main.cpp)
target_link_libraries(mostro PRIVATE Qt6::Widgets ${lib_dep})

if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
  include(AddQtIosApp.cmake)
  add_qt_ios_app(mostro
   NAME "mostro"
   BUNDLE_IDENTIFIER "mostro.app"
   CUSTOM_PLIST "MacOSXBundleInfo.plist.in"
   CODE_SIGN_IDENTITY "iPhone Developer"
   TEAM_ID "Space Research Software LLC"
 )
else()
  set_target_properties(mostro PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
endif()

