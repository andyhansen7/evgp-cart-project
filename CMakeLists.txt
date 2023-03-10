cmake_minimum_required (VERSION 3.10)
project (evgp-cart)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread -Wall -Wextra --coverage -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -Wall -Wextra")

# Add all libraries to the include path
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" )
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/admin/clearpath_hub/inc/inc-pub")

# Send all executables to build/[debug|release]/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Link Teknic's built library
add_library(sFoundation SHARED IMPORTED)
set_target_properties(sFoundation PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/admin/clearpath_hub/sFoundation/libsFoundation20.so)

# Link ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Targets
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/run" )