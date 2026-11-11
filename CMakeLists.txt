
cmake_minimum_required(VERSION 3.16)

project(ExtendibleHashing LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build executable
add_executable(extendible_hash
    src/main.cpp
    src/extendible_hash.cpp
)

# Include header files
target_include_directories(extendible_hash PRIVATE include)

# Enable compiler warnings
target_compile_options(extendible_hash PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
