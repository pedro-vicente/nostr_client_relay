message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "CMAKE_PREFIX_PATH is " ${CMAKE_PREFIX_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Quick REQUIRED)
find_package(Qt6 COMPONENTS QuickControls2 REQUIRED)

if(APPLE)
  if (APPLE_IOS_TARGET)
    set (CMAKE_PREFIX_PATH "/Users/pvn/Qt6.5.2/6.5.2/ios")
  else()
    set (CMAKE_PREFIX_PATH "/Users/pvn/Qt6.5.2/6.5.2/macos")
  endif()
endif()

qt_add_executable(mostro main.cpp)

qt_add_qml_module(mostro
    URI qml
    VERSION 1.0
    QML_FILES main.qml
    SOURCES user.h user.cpp model.cpp model.h
)

target_link_libraries(mostro
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Qml
    ${lib_dep}
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
  message(STATUS "Mobile iOS build ")
  include(AddQtIosApp.cmake)
  add_qt_ios_app(mostro
   NAME "mostro"
   CUSTOM_PLIST "MacOSXBundleInfo.plist.in"
   CODE_SIGN_IDENTITY "iPhone Developer"
 )
else()
  set_target_properties(mostro PROPERTIES WIN32_EXECUTABLE ON MACOSX_BUNDLE ON)
endif()

