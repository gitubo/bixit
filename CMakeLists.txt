cmake_minimum_required(VERSION 3.14)
project(bixit)

add_compile_options(-fPIC -O3)
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
target_include_directories(bixit PUBLIC include ${LUAJIT_INCLUDE_DIRS} /third_party/json-3.11.2/include)

# Linka le librerie necessarie
target_link_libraries(bixit PRIVATE ${LUAJIT_LIBRARIES} -ldl -lm)
