cmake_minimum_required(VERSION 3.10)

project(callable)

add_library(libcallable INTERFACE)
target_compile_features(libcallable INTERFACE cxx_std_17)
target_include_directories(libcallable INTERFACE include)

add_executable(catch2_unit_tests
    tests/framework/main.cpp
    tests/assign.cpp
    tests/call.cpp
    tests/construct.cpp
    tests/destroy.cpp
)
target_link_libraries(catch2_unit_tests libcallable)

enable_testing()

add_test(NAME catch2 COMMAND catch2_unit_tests)