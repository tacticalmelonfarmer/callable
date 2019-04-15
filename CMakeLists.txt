cmake_minimum_required(VERSION 3.11)

project(callable)

add_library(callable INTERFACE)
target_compile_features(callable INTERFACE cxx_std_17)
target_include_directories(callable INTERFACE include)

if(BUILD_UNIT_TESTS)

add_executable(construct tests/construct.cpp)
target_link_libraries(construct callable)

add_executable(destroy tests/destroy.cpp)
target_link_libraries(destroy callable)

add_executable(assign tests/assign.cpp)
target_link_libraries(assign callable)

add_executable(call tests/call.cpp)
target_link_libraries(call callable)

endif()