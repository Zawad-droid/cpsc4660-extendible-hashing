
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


# Enable automated testing
enable_testing()

# Separate integration test executable
add_executable(extendible_hash_integration_tests
    tests/tests.cpp
    src/extendible_hash.cpp
)

target_include_directories(
    extendible_hash_integration_tests PRIVATE include
)

target_compile_options(
    extendible_hash_integration_tests PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# Register the test with CTest
add_test(
    NAME extendible_hash_integration
    COMMAND extendible_hash_integration_tests
)
