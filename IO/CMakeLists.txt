cmake_minimum_required(VERSION 3.16)

project(IO LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(IO SHARED
  IO_global.h
  io.cpp
  io.h
  arquivo.h
)

#target_link_libraries(IO PRIVATE Core)

target_compile_definitions(IO PRIVATE IO_LIBRARY)
