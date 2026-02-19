#	--View--

cmake_minimum_required(VERSION 3.16)
project(View LANGUAGES CXX)

# Garante que o Qt processe o Q_OBJECT
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# 1. Mude para STATIC para evitar problemas de exportação de símbolos (DLL)
add_library(View STATIC
    wcad.h
    wcad.cpp
)

target_link_libraries(View PRIVATE Qt6::Widgets)

# 2. Isso permite que o main.cpp use #include "wcad.h" diretamente
# ou que os arquivos dentro de View se achem sem ../
target_include_directories(View PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
