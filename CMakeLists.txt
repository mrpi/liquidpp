cmake_minimum_required(VERSION 2.8)
project(liquidpp)

find_package( Boost REQUIRED )
include_directories(${Boost_INCLUDE_DIRS}) 
    
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

add_subdirectory (src) 

enable_testing()
add_subdirectory (test)
