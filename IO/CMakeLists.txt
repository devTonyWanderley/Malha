cmake_minimum_required(VERSION 3.16)

project(IO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(IO SHARED
  IO_global.h
  io.cpp
  io.h
  arquivo.h
  ../Core/geometria.h
)

target_compile_definitions(IO PRIVATE IO_LIBRARY)
