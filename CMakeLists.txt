cmake_minimum_required(VERSION 2.8)
project(liquidpp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_subdirectory (src) 

enable_testing()
add_subdirectory (test)
