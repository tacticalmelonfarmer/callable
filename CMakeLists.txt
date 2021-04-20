cmake_minimum_required(VERSION 3.17)

project(callable)

add_library(callable INTERFACE)
target_compile_features(callable INTERFACE cxx_std_17)
target_include_directories(callable INTERFACE include)

add_executable(
  catch2_unit_tests tests/framework/main.cpp tests/assign.cpp tests/call.cpp
                    tests/construct.cpp tests/destroy.cpp)
target_link_libraries(catch2_unit_tests callable)

enable_testing()

add_test(NAME catch2 COMMAND catch2_unit_tests)
