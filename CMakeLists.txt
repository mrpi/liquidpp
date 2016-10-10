cmake_minimum_required(VERSION 3.0)
project(liquidpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

add_subdirectory (src) 

enable_testing()
add_subdirectory (test)
