cmake_minimum_required(VERSION 3.1)
project(cpu16)

file(GLOB SOURCE_FILES source/*.cpp)
file(GLOB HEADER_FILES source/*.h)

add_executable(cpu16 ${SOURCE_FILES} ${HEADER_FILES})
