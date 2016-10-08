cmake_minimum_required(VERSION 2.6)
project(liquidpp)

add_executable(liquidpp main.cpp)

install(TARGETS liquidpp RUNTIME DESTINATION bin)
