cmake_minimum_required(VERSION 3.11)

project(callable)

enable_testing()

set(PROJECT)

add_library(libcallable INTERFACE)
target_compile_features(libcallable INTERFACE cxx_std_17)
target_include_directories(libcallable INTERFACE include)

file(GLOB TestsSources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/tests "*.cpp")

foreach(TestSource ${TestsSources})

string(REPLACE ".cpp" "" TestName ${TestSource})
add_executable(${TestName} tests/${TestSource})
target_link_libraries(${TestName} libcallable)

endforeach()