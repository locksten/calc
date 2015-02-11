cmake_minimum_required(VERSION 2.6)
project(calc)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -pedantic")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm")

set(EXECUTABLE_NAME "calc")
add_subdirectory(src)
