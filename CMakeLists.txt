cmake_minimum_required (VERSION 3.10)
project (evgp-cart)

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pthread -Wall -Wextra --coverage -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pthread -Wall -Wextra")

# Add all libraries to the include path
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/src" )

### Send all executables to build/[debug|release]/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

### Targets
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")
add_subdirectory( "${CMAKE_CURRENT_SOURCE_DIR}/run" )