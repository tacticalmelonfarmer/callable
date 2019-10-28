cmake_minimum_required(VERSION 3.11)

project(callable)

add_library(libcallable INTERFACE)
target_compile_features(libcallable INTERFACE cxx_std_17)
target_include_directories(libcallable INTERFACE include)

add_executable(assign tests/assign.cpp)
target_link_libraries(assign libcallable)

add_executable(call tests/call.cpp)
target_link_libraries(call libcallable)

add_executable(construct tests/construct.cpp)
target_link_libraries(construct libcallable)

add_executable(destroy tests/destroy.cpp)
target_link_libraries(destroy libcallable)