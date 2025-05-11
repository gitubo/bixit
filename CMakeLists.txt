cmake_minimum_required(VERSION 3.14)
project(bixit)

option(BUILD_TESTING "Build the tests" ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -fno-inline -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Compiling in Debug mode")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Compiling in Release mode")
else()
    message(WARNING "No CMAKE_BUILD_TYPE specified. Defaulting to Release mode.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUAJIT REQUIRED luajit)

# Definisce la libreria condivisa datacarder.so
add_library(bixit SHARED 
    src/logger/Logger.cpp
    src/bitstream/BitStream.cpp
    src/catalog/SchemaCatalog.cpp
)

# Include directories
target_include_directories(bixit PUBLIC ${PROJECT_SOURCE_DIR}/include ${LUAJIT_INCLUDE_DIRS} /third_party/json-3.11.2/include)

# Linka le librerie necessarie
target_link_libraries(bixit PUBLIC ${LUAJIT_LIBRARIES} dl m)

set_target_properties(bixit PROPERTIES OUTPUT_NAME "bixit")


if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()