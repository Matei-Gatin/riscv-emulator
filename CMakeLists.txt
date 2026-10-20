cmake_minimum_required(VERSION 4.1)
project(vm_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(VM_WARNINGS -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)

add_library(vm_core
        src/memory.cpp
        src/registers.cpp
        src/decoder.cpp)
target_include_directories(vm_core PUBLIC src)
target_compile_options(vm_core PRIVATE ${VM_WARNINGS})

add_executable(vm_project src/main.cpp)
target_link_libraries(vm_project PRIVATE vm_core)
target_compile_options(vm_project PRIVATE ${VM_WARNINGS})

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(vm_tests
        tests/memory_test.cpp
        tests/registers_test.cpp
        tests/decoder_test.cpp)
target_link_libraries(vm_tests PRIVATE vm_core GTest::gtest_main)
target_compile_options(vm_tests PRIVATE ${VM_WARNINGS})

include(GoogleTest)
gtest_discover_tests(vm_tests)
