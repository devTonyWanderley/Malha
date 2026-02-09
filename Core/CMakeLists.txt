cmake_minimum_required(VERSION 3.16)

project(Core LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(Core SHARED
  Core_global.h
  core.cpp
  core.h
  geometria.h
)

target_link_libraries(Core PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(Core PRIVATE CORE_LIBRARY)
