cmake_minimum_required(VERSION 2.8)
project(asm16)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(SDL)

add_subdirectory(emu16)
