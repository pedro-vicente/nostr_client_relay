
#//////////////////////////
# sources
#//////////////////////////

set(src ${src})
set(src ${src} hex.h)
set(src ${src} proof.h)
set(src ${src} cursor.h)
set(src ${src} endian.h)
set(src ${src} random.h)
set(src ${src} sha256.h)
set(src ${src} sha256.c)
set(src ${src} base64.h)
set(src ${src} base64.c)
set(src ${src} aes.h)
set(src ${src} aes.c)
set(src ${src} nostri.h)
set(src ${src} nostri.c)
if (MSVC)
  set(src ${src} clock_gettime.h)
endif()

set(src ${src} ${CMAKE_SOURCE_DIR}/ext/secp256k1-0.3.2/include/secp256k1.h)

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
# library 'nostri'
#//////////////////////////

add_library(nostri ${src})
target_link_libraries (nostri secp256k1)
set(lib_dep ${lib_dep} nostri)
set(lib_dep ${lib_dep} secp256k1)
if (MSVC)
  set(lib_dep ${lib_dep} Bcrypt.lib)
endif()

#//////////////////////////
# executables
#//////////////////////////

# tool to generate file 'config.h' , generate from a shell with 
# configurator.exe > config.h
add_executable(configurator configurator.c)

add_executable(nostril nostril.c)
target_link_libraries (nostril ${lib_dep})

#//////////////////////////
# generate  config.h
#//////////////////////////

add_custom_command(
    TARGET configurator
    POST_BUILD
    COMMAND configurator > config.h 
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/src/nostril/config.h" ${CMAKE_SOURCE_DIR}/src/nostril
    COMMENT "Generating config.h"
)

